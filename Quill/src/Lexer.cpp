#include "Lexer.hpp"

#include <iostream>
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

    std::vector<Token> Lexer::Tokenize() {
        std::string        buffer;
        std::vector<Token> tokens;

        int  commentedLine = -1;
        bool comment = false;
        bool string = false;

        while (pos < src.size()) {
            char c = Advance();

            if (c == '\"' && pos > 1 && src[pos - 2] != '\\') {
                string = !string;
            }

            if (string) {
                buffer += c;
                continue;
            }

            if (c == '/' && src[pos] == '/') {
                commentedLine = line;
                Advance();
                c = Advance();
            }

            if (c == '/' && src[pos] == '*') {
                comment = true;
                Advance();
                c = Advance();
            }

            if (c == '*' && src[pos] == '/') {
                comment = false;
                Advance();
                c = Advance();
            }

            if (line == commentedLine || comment) {
                continue;
            }

            const bool delimiter = c == '\n' || TokenizeDelimiter(c);

            if (TokenizeString(Trim(buffer))) {
                tokens.push_back(TokenizeString(Trim(buffer)).value());
                buffer.clear();
            }

            if (delimiter) {
                if (TokenizeInteger(Trim(buffer))) {
                    tokens.push_back(TokenizeInteger(Trim(buffer)).value());
                    buffer.clear();
                }

                if (TokenizeFloat(Trim(buffer))) {
                    tokens.push_back(TokenizeFloat(Trim(buffer)).value());
                    buffer.clear();
                }

                if (Trim(buffer) == "true" || Trim(buffer) == "false") {
                    tokens.push_back({.type = TokenType::Bool, .value = Trim(buffer), .line = line, .col = col});
                    buffer.clear();
                }

                if (Trim(buffer) == "null") {
                    tokens.push_back({.type = TokenType::Null, .value = Trim(buffer), .line = line, .col = col});
                    buffer.clear();
                }
            }

            if (delimiter && !Trim(buffer).empty()) {
                tokens.push_back({.type = TokenType::Ident, .value = Trim(buffer), .line = line, .col = col});
                buffer.clear();
            } else if (!delimiter) {
                buffer.push_back(c);
            }

            if (TokenizeDelimiter(c)) {
                tokens.push_back(TokenizeDelimiter(c).value());
            }
        }

        return tokens;
    }

    char Lexer::Advance() {
        if (src[pos] == '\n') {
            line++;
        } else {
            col++;
        }

        size_t p = pos++;
        return src[p];
    }

    std::optional<Token> Lexer::TokenizeDelimiter(const char &ch) const {
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

    std::optional<Token> Lexer::TokenizeString(const std::string &string) const {
        if (string.empty()) {
            return std::nullopt;
        }

        if (string.size() < 2) {
            return std::nullopt;
        }

        int quoteCount = 0;
        for (size_t i = 0; i < string.size(); i++) {
            if (string[i] == '\"' && string[i - 1] != '\\') quoteCount++;
        }

        if (quoteCount != 2) {
            return std::nullopt;
        }

        if (string[0] != '\"' || string[string.size() - 1] != '\"') {
            return std::nullopt;
        }

        std::string str;
        for (size_t i = 1; i < string.size() - 1; i++) {
            if (string[i] == '\\') {
                str += string[i + 1];
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

    std::optional<Token> Lexer::TokenizeInteger(const std::string &string) const {
        if (string.empty()) {
            return std::nullopt;
        }

        if (string == "0.0") {
        }

        std::string str = string;

        const bool hex = str.substr(0, 2) == "0x" && str.size() <= 18;
        const bool oct = str.substr(0, 2) == "0c" && str.size() <= 34;
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

    std::optional<Token> Lexer::TokenizeFloat(const std::string &string) const {
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
