#ifndef QUILL_QUILL_HPP
#define QUILL_QUILL_HPP

#include "QuillValue.hpp"

namespace Ql {
    using Null = std::monostate;

    class Quill {
        QuillValue node;

    public:
        static Quill Parse(const std::string &data);

        Quill() = default;

        explicit Quill(QuillValue node) : node(std::move(node)) {
        }

        bool operator==(const Quill &other) const;

        Quill operator[](const std::string &key) const { return Get(key); }
        Quill operator[](const Quill &key) const { return Get(key); }
        Quill operator[](const size_t index) const { return Get(index); }

        Quill &operator[](const std::string &key) { return GetMut(key); }
        Quill &operator[](const Quill &key) { return GetMut(key); }
        Quill &operator[](const size_t index) { return GetMut(index); }

        template<typename T>
        Quill &operator=(const T &value) {
            node.value = value;
            return *this;
        }

        [[nodiscard]] bool Contains(const std::string &key) const;
        [[nodiscard]] bool Contains(const Quill &key) const;
        [[nodiscard]] bool Contains(const size_t &index) const;

        [[nodiscard]] Quill Get(const std::string &key) const;
        [[nodiscard]] Quill Get(const Quill &key) const;
        [[nodiscard]] Quill Get(size_t index) const;

        Quill &GetMut(const std::string &key);
        Quill &GetMut(const Quill &key);
        Quill &GetMut(size_t index);

        template<typename T>
        T As() const {
        }

        template<typename T>
        void Push(T value) {
            assert(IsArray());
            std::get<Array>(node.value).Push(QuillValue{.value = value, .type = GetValueType<T>()});
        }

        Quill Pop();

        [[nodiscard]] bool IsNull() const { return std::holds_alternative<std::monostate>(node.value); }
        [[nodiscard]] bool IsStruct() const { return std::holds_alternative<Struct>(node.value); }
        [[nodiscard]] bool IsArray() const { return std::holds_alternative<Array>(node.value); }
        [[nodiscard]] bool IsMap() const { return std::holds_alternative<Map>(node.value); }

        [[nodiscard]] std::string TypeName() const {
            if (IsStruct()) return std::get<Struct>(node.value).name;
            return "";
        }

        [[nodiscard]] std::string ToString(size_t spaces = 2, int floatPrecision = 8, size_t indent = 0) const;
    };

    template<>
    inline int Quill::As<int>() const {
        return static_cast<int>(std::get<int64_t>(node.value));
    }

    template<>
    inline float Quill::As<float>() const {
        if (std::holds_alternative<double>(node.value))
            return static_cast<float>(std::get<double>(node.value));
        return static_cast<float>(std::get<int64_t>(node.value));
    }

    template<>
    inline std::string Quill::As<std::string>() const {
        return std::get<std::string>(node.value);
    }

    template<>
    inline bool Quill::As<bool>() const {
        return std::get<bool>(node.value);
    }
}

#endif //QUILL_QUILL_HPP
