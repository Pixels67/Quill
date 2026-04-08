#ifndef QUILL_QUILLVALUE_HPP
#define QUILL_QUILLVALUE_HPP

#include <map>
#include <string>
#include <vector>
#include <variant>

#include "Token.hpp"

namespace Ql {
    struct QuillValue;

    enum class ValueType {
        Struct,
        Array,
        Map,
        String,
        Integer,
        Float,
        Bool,
        Null,
    };

    struct Struct {
        std::string name;
        std::vector<std::pair<std::string, QuillValue>> fields;

        QuillValue &operator[](const std::string &key);
        QuillValue operator[](const std::string &key) const;

        [[nodiscard]] bool Contains(const std::string &key) const;

        bool operator==(const Struct &other) const;
    };

    struct Array {
        std::vector<QuillValue> elements;
        ValueType arrayType = ValueType::Null;

        void Push(const QuillValue &value);
        QuillValue Pop();

        QuillValue &operator[](size_t index);
        QuillValue operator[](size_t index) const;

        [[nodiscard]] bool Contains(size_t index) const;

        bool operator==(const Array &other) const;
    };

    struct Map {
        std::vector<std::pair<QuillValue, QuillValue> > entries;
        ValueType keyType = ValueType::Null;

        QuillValue &operator[](const QuillValue &key);
        QuillValue operator[](const QuillValue &key) const;

        [[nodiscard]] bool Contains(const QuillValue &key) const;

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
        ValueType type;
        int line;
        int col;

        bool operator==(const QuillValue &quillValue) const = default;
    };

    template<typename T>
    ValueType GetValueType() {
        if constexpr (std::same_as<T, Struct>) {
            return ValueType::Struct;
        }

        if constexpr (std::same_as<T, Array>) {
            return ValueType::Array;
        }

        if constexpr (std::same_as<T, Map>) {
            return ValueType::Map;
        }

        if constexpr (std::same_as<T, std::string>) {
            return ValueType::String;
        }

        if constexpr (std::same_as<T, int64_t> || std::same_as<T, int32_t>) {
            return ValueType::Integer;
        }

        if constexpr (std::same_as<T, double> || std::same_as<T, float>) {
            return ValueType::Float;
        }

        if constexpr (std::same_as<T, bool>) {
            return ValueType::Bool;
        }

        return ValueType::Null;
    }
} // Quill

#endif //QUILL_QUILLVALUE_HPP
