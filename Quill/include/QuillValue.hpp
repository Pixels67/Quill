#ifndef QUILL_QUILLVALUE_HPP
#define QUILL_QUILLVALUE_HPP

#include <string>
#include <vector>
#include <unordered_map>
#include <variant>

namespace Ql {
    struct QuillValue;

    struct Struct {
        std::string                                 name;
        std::unordered_map<std::string, QuillValue> fields;

        bool operator==(const Struct &other) const;
    };

    struct Array {
        std::vector<QuillValue> elements;

        bool operator==(const Array &other) const;
    };

    struct Map {
        std::vector<std::pair<QuillValue, QuillValue> > entries;

        bool operator==(const Map &other) const;
    };

    using AstValue = std::variant<
        Struct,
        Array,
        Map,
        std::string,
        int64_t,
        double,
        bool,
        std::monostate
    >;

    struct QuillValue {
        AstValue value;
        int      line;
        int      col;

        bool operator==(const QuillValue &quillValue) const = default;
    };

    inline bool Struct::operator==(const Struct &other) const {
        return name == other.name && fields == other.fields;
    }

    inline bool Array::operator==(const Array &other) const {
        return elements == other.elements;
    }

    inline bool Map::operator==(const Map &other) const {
        return entries == other.entries;
    }
} // Quill

#endif //QUILL_QUILLVALUE_HPP
