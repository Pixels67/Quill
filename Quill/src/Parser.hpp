#ifndef QUILL_PARSER_HPP
#define QUILL_PARSER_HPP

#include <string>
#include <vector>

#include "QuillValue.hpp"
#include "Token.hpp"

namespace Ql {
    class Parser {
        std::vector<Token> tokens;
        size_t pos = 0;

    public:
        explicit Parser(std::vector<Token> tokens) : tokens(std::move(tokens)) {
        }

        QuillValue Parse();

    private:
        Token &Current();
        Token &Peek(int offset = 1);
        Token Consume();
        Token Expect(TokenType);

        QuillValue ParseValue();
        Struct ParseStruct(const std::string &name);
        Array ParseArray();
        Map ParseMap();
    };
} // Quill

#endif //QUILL_PARSER_HPP
