#include "QuillValue.hpp"

#include "QuillError.hpp"

namespace Ql {
    QuillValue &Struct::operator[](const std::string &key) {
        for (auto &[k, v]: fields) {
            if (k == key) {
                return v;
            }
        }

        fields.emplace_back(key, QuillValue{.value = std::monostate{}});
        return fields.back().second;
    }

    QuillValue Struct::operator[](const std::string &key) const {
        for (auto &[k, v]: fields) {
            if (k == key) {
                return v;
            }
        }

        throw QuillError("Index out of range");
    }

    bool Struct::Contains(const std::string &key) const {
        return std::any_of(fields.begin(), fields.end(), [&](auto &pair) { return pair.first == key; });
    }

    bool Struct::operator==(const Struct &other) const {
        return name == other.name && fields == other.fields;
    }

    bool Array::operator==(const Array &other) const {
        return elements == other.elements;
    }

    QuillValue &Map::operator[](const QuillValue &key) {
        if (keyType == ValueType::Null) {
            keyType = key.type;
        }

        if (keyType != key.type) {
            throw QuillError("Cannot assign maps keys of different types");
        }

        for (auto &[k, v]: entries) {
            if (k.value == key.value) {
                return v;
            }
        }

        entries.emplace_back(key, QuillValue{.value = std::monostate{}});
        return entries.back().second;
    }

    QuillValue Map::operator[](const QuillValue &key) const {
        for (auto &[k, v]: entries) {
            if (k == key) {
                return v;
            }
        }

        throw QuillError("Index out of range");
    }

    bool Map::Contains(const QuillValue &key) const {
        return std::any_of(entries.begin(), entries.end(), [&](auto &pair) { return pair.first == key; });
    }

    bool Map::operator==(const Map &other) const {
        return entries == other.entries;
    }

    void Array::Push(const QuillValue &value) {
        if (arrayType == ValueType::Null) {
            arrayType = value.type;
        }

        if (arrayType != value.type) {
            throw QuillError("Cannot assign arrays values of different types");
        }

        elements.push_back(value);
    }

    QuillValue Array::Pop() {
        QuillValue val = elements.back();
        elements.pop_back();

        return val;
    }

    QuillValue &Array::operator[](const size_t index) {
        if (index >= elements.size()) {
            throw QuillError("Index out of range");
        }

        return elements[index];
    }

    QuillValue Array::operator[](const size_t index) const {
        if (index >= elements.size()) {
            throw QuillError("Index out of range");
        }

        return elements[index];
    }

    bool Array::Contains(const size_t index) const {
        return index < elements.size();
    }
}
