#ifndef QUILL_QUILLERROR_HPP
#define QUILL_QUILLERROR_HPP

#include <stdexcept>
#include <string>

namespace Ql {
    class QuillError : public std::runtime_error {
    public:
        explicit QuillError(const std::string &error) : std::runtime_error(error) {
        }
    };
}

#endif //QUILL_QUILLERROR_HPP
