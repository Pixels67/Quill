#ifndef QUILL_QUILL_HPP
#define QUILL_QUILL_HPP

#include "QuillValue.hpp"

namespace Ql {
    class Quill {
        QuillValue node;

    public:
        static Quill Parse(const std::string &data);

        explicit Quill(QuillValue node) : node(std::move(node)) {
        }

        bool operator==(const Quill &other) const {
            return std::visit([&]<typename T0, typename T1>(const T0 &a, const T1 &b) -> bool {
                using A = std::decay_t<T0>;
                using B = std::decay_t<T1>;
                if constexpr (!std::is_same_v<A, B>) return false;
                else return a == b;
            }, node.value, other.node.value);
        }

        Quill operator[](const std::string &key) const {
            if (IsStruct()) {
                const auto &[name, fields] = std::get<Struct>(node.value);
                const auto  it = fields.find(key);

                if (it == fields.end())
                    assert(false);
                return Quill{it->second};
            }

            if (IsMap()) {
                const auto &[elements] = std::get<Map>(node.value);
                for (const auto &[k, val]: elements) {
                    if (key == std::get<std::string>(k.value)) {
                        return Quill(val);
                    }
                }
            }

            assert(false);
        }

        Quill operator[](const Quill &key) const {
            const auto &[elements] = std::get<Map>(node.value);
            for (const auto &[k, val]: elements) {
                if (key == Quill(k)) {
                    return Quill(val);
                }
            }

            assert(false);
        }

        Quill operator[](const size_t index) const {
            if (IsArray()) {
                const auto &[elements] = std::get<Array>(node.value);
                return Quill{elements.at(index)};
            }

            if (IsMap()) {
                const auto &[elements] = std::get<Map>(node.value);
                for (const auto &[k, val]: elements) {
                    if (index == std::get<int64_t>(k.value)) {
                        return Quill(val);
                    }
                }
            }

            assert(false);
        }


        template<typename T>
        T As() const;

        [[nodiscard]] bool IsNull() const { return std::holds_alternative<std::monostate>(node.value); }
        [[nodiscard]] bool IsStruct() const { return std::holds_alternative<Struct>(node.value); }
        [[nodiscard]] bool IsArray() const { return std::holds_alternative<Array>(node.value); }
        [[nodiscard]] bool IsMap() const { return std::holds_alternative<Map>(node.value); }

        [[nodiscard]] std::string TypeName() const {
            if (IsStruct()) return std::get<Struct>(node.value).name;
            return "";
        }
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
