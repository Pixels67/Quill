#include "Parser.hpp"

#include <cassert>

namespace Ql {
    QuillValue Parser::Parse() {
        QuillValue node = ParseValue();
        if (pos != tokens.size()) {
            assert(false);
        }

        return node;
    }

    Token &Parser::Current() {
        if (pos >= tokens.size()) {
            assert(false);
        }

        return tokens[pos];
    }

    Token &Parser::Peek(const int offset) {
        return tokens[pos + offset];
    }

    Token Parser::Consume() {
        pos++;
        return tokens[pos - 1];
    }

    Token Parser::Expect(const TokenType type) {
        if (tokens[pos].type != type) {
            assert(false);
        }

        pos++;
        return tokens[pos - 1];
    }

    QuillValue Parser::ParseValue() {
        if (Current().type == TokenType::Ident) {
            const std::string name = Consume().value;

            if (Current().type == TokenType::LBrace) {
                return {ParseStruct(name)};
            }

            if (Current().type == TokenType::LParen) {
                return {ParseBuiltin(name)};
            }

            if (Current().type == TokenType::DoubleColon) {
                Consume();
                return {Enum{name, Consume().value}};
            }

            assert(false);
        }

        if (Current().type == TokenType::LBrace) return {ParseStruct("")};

        if (Current().type == TokenType::LBracket) {
            int       idx = 1;
            TokenType type = Peek(idx).type;
            int       nesting = 0;
            while (true) {
                if (type == TokenType::Colon && nesting == 0) return {ParseMap()};
                if (type == TokenType::RBracket && nesting == 0) break;

                if (type == TokenType::LBracket || type == TokenType::LBrace) nesting++;
                if (type == TokenType::RBracket || type == TokenType::RBrace) nesting--;

                type = Peek(++idx).type;
            }

            return {ParseArray()};
        }

        if (Current().type == TokenType::String) return {Consume().value};
        if (Current().type == TokenType::Integer) return {std::stoll(Consume().value)};
        if (Current().type == TokenType::Float) return {std::stod(Consume().value)};
        if (Current().type == TokenType::Bool) return {Consume().value == "true"};
        if (Current().type == TokenType::Null) {
            Consume();
            return {std::monostate{}};
        };

        assert(false);
    }

    Struct Parser::ParseStruct(const std::string &name) {
        Struct node;
        node.name = name;

        Expect(TokenType::LBrace);
        while (true) {
            if (Current().type == TokenType::RBrace) {
                Consume();
                break;
            }

            std::string fieldName = Expect(TokenType::Ident).value;
            Expect(TokenType::Colon);

            node.fields[fieldName] = ParseValue();

            if (Current().type == TokenType::Comma) {
                Consume();
            } else if (Current().type == TokenType::RBrace) {
                Consume();
                break;
            } else {
                assert(false);
            }
        }

        return node;
    }

    Builtin Parser::ParseBuiltin(const std::string &name) {
        Builtin node;
        node.name = name;

        Expect(TokenType::LParen);
        while (true) {
            if (Current().type == TokenType::Integer) {
                node.args.push_back({std::stoll(Consume().value)});
            } else if (Current().type == TokenType::Float) {
                node.args.push_back({std::stod(Consume().value)});
            } else {
                assert(false);
            }

            if (Current().type == TokenType::Comma) {
                Consume();
            } else if (Current().type == TokenType::RParen) {
                Consume();
                break;
            } else {
                assert(false);
            }
        }

        return node;
    }

    Array Parser::ParseArray() {
        Array node;
        Expect(TokenType::LBracket);
        while (true) {
            if (Current().type == TokenType::RBracket) {
                Consume();
                break;
            }

            node.elements.push_back(ParseValue());

            if (Current().type == TokenType::Comma) {
                Consume();
            } else if (Current().type == TokenType::RBracket) {
                Consume();
                break;
            } else {
                assert(false);
            }
        }

        return node;
    }

    Map Parser::ParseMap() {
        Map node;

        Expect(TokenType::LBracket);
        while (true) {
            if (Current().type == TokenType::RBracket) {
                Consume();
                break;
            }

            if (Current().type == TokenType::Colon) {
                Consume();
                Expect(TokenType::RBracket);
                return node;
            }

            QuillValue key = ParseValue();

            Expect(TokenType::Colon);

            QuillValue value = ParseValue();

            node.entries.emplace_back(key, value);

            if (Current().type == TokenType::Comma) {
                Consume();
            } else if (Current().type == TokenType::RBracket) {
                Consume();
                break;
            } else {
                assert(false);
            }
        }

        return node;
    }
} // Quill
