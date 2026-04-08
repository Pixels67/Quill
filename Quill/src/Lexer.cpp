#include "Lexer.hpp"

#include <vector>

namespace Ql {
    std::string Trim(std::string str) {
        if (str.empty()) {
            return str;
        }

        while (str[0] == ' ' && !str.empty()) {
            str = str.substr(1);
        }

        while (str.back() == ' ' && !str.empty()) {
            str = str.substr(0, str.size() - 1);
        }

        return str;
    }

    bool IsDelimiter(const char c) {
        return c == '{' || c == '}' || c == '[' || c == ']' || c == ':' || c == ',' || c == ' ' || c == '\n';
    }

    std::vector<Token> Lexer::Tokenize() {
        std::vector<Token> tokenStrings;

        bool string = false;
        bool comment = false;

        std::string buffer;

        while (Current() != '\0') {
            int col = currentCol;
            int line = currentLine;

            // Strings
            if (Current() == '\"' && Peek(-1) != '\\') {
                string = !string;
            }

            if (string) {
                buffer += Current();
                Advance();
                continue;
            }

            // Comments
            if (Current() == '/' && Peek(1) == '/') {
                SkipLine();
            }

            if (Current() == '/' && Peek(1) == '*') {
                comment = true;
            }

            if (Current() == '*' && Peek(1) == '/') {
                comment = false;
                Advance(2);
            }

            if (comment) {
                Advance();
                continue;
            }

            // Delimiters
            if (IsDelimiter(Current())) {
                if (!buffer.empty()) {
                    int c = col - buffer.size() + 1;
                    tokenStrings.push_back({.value = buffer, .line = line, .col = c});
                    buffer.clear();
                }

                if (Current() != ' ' && Current() != '\n') {
                    std::string str;
                    str.push_back(Current());
                    tokenStrings.push_back({.value = str, .line = line, .col = col - 1});
                }

                Advance();
                continue;
            }

            // Everything else
            buffer += Current();
            Advance();
        }

        std::vector<Token> tokens;

        for (const auto &token: tokenStrings) {
            auto [type, str, line, col] = token;
            if (str.size() == 1 && TokenizeDelimiter(str[0], line, col)) {
                tokens.push_back(TokenizeDelimiter(str[0], line, col).value());
            } else if (TokenizeInteger(str, line, col)) {
                tokens.push_back(TokenizeInteger(str, line, col).value());
            } else if (TokenizeFloat(str, line, col)) {
                tokens.push_back(TokenizeFloat(str, line, col).value());
            } else if (TokenizeString(str, line, col)) {
                tokens.push_back(TokenizeString(str, line, col).value());
            } else {
                tokens.push_back({.type = TokenType::Ident, .value = str, .line = line, .col = col});
            }
        }

        return tokens;
    }

    char Lexer::Current() const {
        return src[pos];
    }

    char Lexer::Advance(const size_t offset) {
        if (offset == 0) {
            return Current();
        }

        if (src[pos] == '\n') {
            currentLine++;
            currentCol = 0;
        } else {
            currentCol++;
        }

        pos++;
        return Advance(offset - 1);
    }

    char Lexer::Peek(const size_t offset) const {
        if (pos + offset >= src.size() || pos + offset < 0) {
            return '\0';
        }

        return src[pos + offset];
    }

    void Lexer::SkipLine() {
        const int line = currentLine;
        while (currentLine == line) {
            Advance();
        }
    }

    std::optional<Token> Lexer::TokenizeDelimiter(const char &ch, int line, int col) const {
        if (ch == '{') {
            return Token{.type = TokenType::LBrace, .value = "{", .line = line, .col = col};
        }

        if (ch == '}') {
            return Token{.type = TokenType::RBrace, .value = "}", .line = line, .col = col};
        }

        if (ch == '[') {
            return Token{.type = TokenType::LBracket, .value = "[", .line = line, .col = col};
        }

        if (ch == ']') {
            return Token{.type = TokenType::RBracket, .value = "]", .line = line, .col = col};
        }

        if (ch == ',') {
            return Token{.type = TokenType::Comma, .value = ",", .line = line, .col = col};
        }

        if (ch == ':') {
            return Token{.type = TokenType::Colon, .value = ":", .line = line, .col = col};
        }

        return std::nullopt;
    }

    std::optional<Token> Lexer::TokenizeString(const std::string &string, int line, int col) const {
        if (string.empty()) {
            return std::nullopt;
        }

        if (string.size() < 2) {
            return std::nullopt;
        }

        int quoteCount = 0;
        for (size_t i = 0; i < string.size(); i++) {
            if (string[i] == '\"' && (i == 0 || string[i - 1] != '\\')) quoteCount++;
        }

        if (quoteCount != 2) {
            return std::nullopt;
        }

        if (string[0] != '\"' || string[string.size() - 1] != '\"') {
            return std::nullopt;
        }

        std::string str;
        for (size_t i = 1; i < string.size() - 1; i++) {
            if (string[i] == '\\' && string[i + 1] == '\"') {
                str += '\"';
                i++;
            } else if (string[i] == '\\' && string[i + 1] == '\\') {
                str += '\\';
                i++;
            } else if (string[i] == '\\' && string[i + 1] == 'n') {
                str += '\n';
                i++;
            } else {
                str += string[i];
            }
        }

        return Token{
            .type  = TokenType::String,
            .value = str,
            .line  = line,
            .col   = col,
        };
    }

    std::optional<Token> Lexer::TokenizeInteger(const std::string &string, int line, int col) const {
        if (string.empty()) {
            return std::nullopt;
        }

        if (string == "0.0") {
        }

        std::string str = string;

        const bool hex = str.substr(0, 2) == "0x" && str.size() <= 18;
        const bool oct = str.substr(0, 2) == "0o" && str.size() <= 34;
        const bool bin = str.substr(0, 2) == "0b" && str.size() <= 66;
        const bool dec = !hex && !oct && !bin;

        if (!dec) {
            str = str.substr(2);
        }

        const bool valid = std::all_of(str.begin(), str.end(), [&](const char c) {
            if (bin && c >= '0' && c <= '1') return true;
            if (oct && c >= '0' && c <= '8') return true;
            if (dec && c >= '0' && c <= '9') return true;

            bool validHex = (c >= '0' && c <= '9') || (c >= 'A' && c <= 'F') || (c >= 'a' && c <= 'f');
            if (hex && validHex) return true;

            return false;
        });

        if (!valid) {
            return std::nullopt;
        }

        return Token{
            .type  = TokenType::Integer,
            .value = string,
            .line  = line,
            .col   = col,
        };
    }

    std::optional<Token> Lexer::TokenizeFloat(const std::string &string, int line, int col) const {
        if (string.empty()) {
            return std::nullopt;
        }

        int pointCount = 0;
        for (const char c: string) {
            if (c == '.') pointCount++;
        }

        if (pointCount > 1) {
            return std::nullopt;
        }

        const bool valid = std::all_of(string.begin(), string.end(), [&](const char c) {
            if (c >= '0' && c <= '9') return true;
            if (c == '.') return true;

            return false;
        });

        if (!valid) {
            return std::nullopt;
        }

        return Token{
            .type  = TokenType::Float,
            .value = string,
            .line  = line,
            .col   = col,
        };
    }
} // Quill
