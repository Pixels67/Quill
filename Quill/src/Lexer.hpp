#ifndef QUILL_LEXER_HPP
#define QUILL_LEXER_HPP

#include <optional>
#include <string_view>

#include "Token.hpp"

namespace Ql {
    std::string Trim(std::string str);
    bool        IsDelimiter(char c);

    class Lexer {
        std::string_view src;
        size_t           pos = 0;
        int              currentLine = 1;
        int              currentCol = 1;

    public:
        explicit Lexer(const std::string_view src) : src(src) {
        }

        std::vector<Token> Tokenize();

    private:
        char Current() const;
        char Advance(size_t offset = 1);
        char Peek(size_t offset = 1) const;
        void SkipLine();

        [[nodiscard]] std::optional<Token> TokenizeDelimiter(const char &ch, int line, int col) const;
        [[nodiscard]] std::optional<Token> TokenizeString(const std::string &string, int line, int col) const;
        [[nodiscard]] std::optional<Token> TokenizeInteger(const std::string &string, int line, int col) const;
        [[nodiscard]] std::optional<Token> TokenizeFloat(const std::string &string, int line, int col) const;
    };
} // Quill

#endif //QUILL_LEXER_HPP
