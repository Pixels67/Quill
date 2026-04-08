#ifndef QUILL_TOKEN_HPP
#define QUILL_TOKEN_HPP

#include <string>

namespace Ql {
    enum class TokenType {
        // Literals
        String,
        Integer,
        Float,
        Bool,
        Null,

        // Identifiers & keywords
        Ident,
        DoubleColon,

        // Brackets
        LBrace,
        RBrace,
        LParen,
        RParen,
        LBracket,
        RBracket,

        // Punctuation
        Colon,
        Comma,
    };

    struct Token {
        TokenType   type;
        std::string value;
        int         line;
        int         col;
    };
}

#endif //QUILL_TOKEN_HPP
