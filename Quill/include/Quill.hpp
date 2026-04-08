#ifndef QUILL_QUILL_HPP
#define QUILL_QUILL_HPP
#include <array>

#include "QuillValue.hpp"

namespace Ql {
    class Quill {
        QuillValue node;

    public:
        static Quill Parse(const std::string &data);

        explicit Quill(QuillValue node) : node(std::move(node)) {
        }

        bool operator==(const Quill& other) const {
            return std::visit([&]<typename T0, typename T1>(const T0& a, const T1& b) -> bool {
                using A = std::decay_t<T0>;
                using B = std::decay_t<T1>;
                if constexpr (!std::is_same_v<A, B>) return false;
                else return a == b;
            }, node.value, other.node.value);
        }

        Quill operator[](const std::string &key) const {
            const auto &[name, fields] = std::get<Struct>(node.value);
            const auto  it = fields.find(key);

            if (it == fields.end())
                assert(false);
            return Quill{it->second};
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

        Enum AsEnum() const { return {std::get<Enum>(node.value).type, std::get<Enum>(node.value).value}; }

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

    template<>
    inline std::array<float, 2> Quill::As<std::array<float, 2> >() const {
        const auto &[name, args] = std::get<Builtin>(node.value);
        assert(name == "xy");
        return std::array{
            Quill{args[0]}.As<float>(),
            Quill{args[1]}.As<float>()
        };
    }

    template<>
    inline std::array<float, 3> Quill::As<std::array<float, 3> >() const {
        const auto &[name, args] = std::get<Builtin>(node.value);
        assert(name == "xyz");
        return std::array{
            Quill{args[0]}.As<float>(),
            Quill{args[1]}.As<float>(),
            Quill{args[2]}.As<float>()
        };
    }

    template<>
    inline std::array<float, 4> Quill::As<std::array<float, 4> >() const {
        const auto &[name, args] = std::get<Builtin>(node.value);
        assert(name == "xyzw");
        return std::array{
            Quill{args[0]}.As<float>(),
            Quill{args[1]}.As<float>(),
            Quill{args[2]}.As<float>(),
            Quill{args[3]}.As<float>()
        };
    }

    template<>
    inline std::array<int, 2> Quill::As<std::array<int, 2> >() const {
        const auto &[name, args] = std::get<Builtin>(node.value);
        assert(name == "rg");
        return std::array{
            Quill{args[0]}.As<int>(),
            Quill{args[1]}.As<int>()
        };
    }

    template<>
    inline std::array<int, 3> Quill::As<std::array<int, 3> >() const {
        const auto &[name, args] = std::get<Builtin>(node.value);
        assert(name == "rgb");
        return std::array{
            Quill{args[0]}.As<int>(),
            Quill{args[1]}.As<int>(),
            Quill{args[2]}.As<int>()
        };
    }

    template<>
    inline std::array<int, 4> Quill::As<std::array<int, 4> >() const {
        const auto &[name, args] = std::get<Builtin>(node.value);
        assert(name == "rgba");
        return std::array{
            Quill{args[0]}.As<int>(),
            Quill{args[1]}.As<int>(),
            Quill{args[2]}.As<int>(),
            Quill{args[3]}.As<int>()
        };
    }
}

#endif //QUILL_QUILL_HPP
