#include "Quill.hpp"

#include <array>

#include "Lexer.hpp"
#include "Parser.hpp"

namespace Ql {
    Quill Quill::Parse(const std::string& data) {
        return Quill(Parser(Lexer(data).Tokenize()).Parse());
    }
}
