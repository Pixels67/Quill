#include "Parser.hpp"

#include "QuillError.hpp"

namespace Ql {
    QuillValue Parser::Parse() {
        QuillValue node = ParseValue();
        if (pos != tokens.size()) {
            const std::string line = std::to_string(tokens[pos].line);
            const std::string col = std::to_string(tokens[pos].col);
            throw QuillError("Extra token on " + line + ":" + col);
        }

        return node;
    }

    Token &Parser::Current() {
        if (pos > tokens.size()) {
            throw QuillError("Missing tokens at end of file");
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
            const std::string line = std::to_string(tokens[pos].line);
            const std::string col = std::to_string(tokens[pos].col);
            throw QuillError("Unexpected token: " + tokens[pos].value + " on " + line + ":" + col);
        }

        pos++;
        return tokens[pos - 1];
    }

    QuillValue Parser::ParseValue() {
        if (Current().type == TokenType::Ident) {
            const std::string name = Consume().value;

            if (Current().type == TokenType::LBrace) {
                return {ParseStruct(name), Current().line, Current().col};
            }

            const std::string line = std::to_string(tokens[pos - 1].line);
            const std::string col = std::to_string(tokens[pos - 1].col + tokens[pos - 1].value.size());
            throw QuillError("Expected '{' after identifier '" + name + "' on " + line + ":" + col);
        }

        if (Current().type == TokenType::LBrace) return {ParseStruct(""), Current().line, Current().col};

        if (Current().type == TokenType::LBracket) {
            int       idx = 1;
            TokenType type = Peek(idx).type;
            int       nesting = 0;
            while (true) {
                if (type == TokenType::Colon && nesting == 0) return {ParseMap(), Current().line, Current().col};
                if (type == TokenType::RBracket && nesting == 0) break;

                if (type == TokenType::LBracket || type == TokenType::LBrace) nesting++;
                if (type == TokenType::RBracket || type == TokenType::RBrace) nesting--;

                type = Peek(++idx).type;
            }

            return {ParseArray()};
        }

        if (Current().type == TokenType::String) return {Consume().value, Current().line, Current().col};
        if (Current().type == TokenType::Integer) {
            int base = 10;
            if (Current().value.substr(0, 2) == "0x") {
                base = 16;
            }
            if (Current().value.substr(0, 2) == "0o") {
                base = 8;
            }
            if (Current().value.substr(0, 2) == "0b") {
                base = 2;
            }

            if (base == 10) {
                return {std::stoll(Consume().value), Current().line, Current().col};
            }

            return {std::stoll(Consume().value.substr(2), nullptr, base), Current().line, Current().col};
        }

        if (Current().type == TokenType::Float) return {std::stod(Consume().value), Current().line, Current().col};
        if (Current().type == TokenType::Bool) return {Consume().value == "true", Current().line, Current().col};
        if (Current().type == TokenType::Null) {
            Consume();
            return {std::monostate{}, Current().line, Current().col};
        };

        const std::string line = std::to_string(tokens[pos].line);
        const std::string col = std::to_string(tokens[pos].col + 1);
        throw QuillError("Expected field value after ':' on " + line + ":" + col);
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
                const std::string line = std::to_string(node.fields[fieldName].line);
                const std::string col = std::to_string(node.fields[fieldName].col);
                throw QuillError("Expected ',' or '}' after identifier '" + fieldName + "' on " + line + ":" + col);
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
                const std::string line = std::to_string(node.elements.back().line);
                const std::string col = std::to_string(node.elements.back().col);
                throw QuillError(
                    "Expected ',' or '}' after array element " + std::to_string(node.elements.size() - 1)
                    + " on " + line + ":" + col
                );
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
                const std::string line = std::to_string(node.entries.back().second.line);
                const std::string col = std::to_string(node.entries.back().second.col);
                throw QuillError("Expected ',' or '}' after map element on " + line + ":" + col);
            }
        }

        return node;
    }
} // Quill
