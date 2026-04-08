#ifndef QUILL_LEXER_HPP
#define QUILL_LEXER_HPP

#include <optional>
#include <string_view>

#include "Token.hpp"

namespace Ql {
    std::string Trim(std::string str);

    class Lexer {
        std::string_view src;
        size_t           pos = 0;
        int              line = 1;
        int              col = 1;

    public:
        explicit Lexer(const std::string_view src) : src(src) {
        }

        std::vector<Token> Tokenize();

    private:
        char Advance();

        [[nodiscard]] std::optional<Token> TokenizeDelimiter(const char &ch) const;
        [[nodiscard]] std::optional<Token> TokenizeString(const std::string &string) const;
        [[nodiscard]] std::optional<Token> TokenizeInteger(const std::string &string) const;
        [[nodiscard]] std::optional<Token> TokenizeFloat(const std::string &string) const;
    };
} // Quill

#endif //QUILL_LEXER_HPP
