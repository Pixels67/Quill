#include "Quill.hpp"

#include <array>
#include <iomanip>
#include <iostream>
#include <sstream>

#include "Lexer.hpp"
#include "Parser.hpp"

namespace Ql {
    Quill Quill::Parse(const std::string &data) {
        return Quill(Parser(Lexer(data).Tokenize()).Parse());
    }

    bool Quill::operator==(const Quill &other) const {
        return std::visit([&]<typename T0, typename T1>(const T0 &a, const T1 &b) -> bool {
            using A = std::decay_t<T0>;
            using B = std::decay_t<T1>;
            if constexpr (!std::is_same_v<A, B>) return false;
            else return a == b;
        }, node.value, other.node.value);
    }

    bool Quill::Contains(const std::string &key) const {
        if (IsStruct()) {
            return std::get<Struct>(node.value).Contains(key);
        }

        if (IsMap()) {
            return std::get<Map>(node.value).Contains(QuillValue{key});
        }

        return false;
    }

    bool Quill::Contains(const Quill &key) const {
        return std::get<Map>(node.value).Contains(key.node);
    }

    bool Quill::Contains(const size_t &index) const {
        if (IsArray()) {
            return std::get<Array>(node.value).Contains(index);
        }

        if (IsMap()) {
            return std::get<Map>(node.value).Contains(QuillValue{static_cast<int>(index)});
        }

        return false;
    }

    Quill Quill::Get(const std::string &key) const {
        if (IsStruct()) {
            return Quill{std::get<Struct>(node.value)[key]};
        }

        if (IsMap()) {
            return Quill{std::get<Map>(node.value)[QuillValue{
                .value = key,
                .type = ValueType::String,
            }]};
        }

        assert(false);
    }

    Quill Quill::Get(const Quill &key) const {
        if (IsMap()) {
            return Quill{std::get<Map>(node.value)[key.node]};
        }

        assert(false);
    }

    Quill Quill::Get(const size_t index) const {
        if (IsArray()) {
            return Quill{std::get<Array>(node.value)[index]};
        }

        if (IsMap()) {
            return Quill{std::get<Map>(node.value)[QuillValue{
                .value = static_cast<int>(index),
                .type = ValueType::Integer,
            }]};
        }

        assert(false);
    }

    Quill &Quill::GetMut(const std::string &key) {
        if (IsStruct()) {
            return reinterpret_cast<Quill &>(std::get<Struct>(node.value)[key]);
        }

        if (IsMap()) {
            return reinterpret_cast<Quill &>(std::get<Map>(node.value)[QuillValue{
                .value = key,
                .type = ValueType::String,
            }]);
        }

        assert(false);
    }

    Quill &Quill::GetMut(const Quill &key) {
        if (IsMap()) {
            return reinterpret_cast<Quill &>(std::get<Map>(node.value)[key.node]);
        }

        assert(false);
    }

    Quill &Quill::GetMut(const size_t index) {
        if (IsArray()) {
            return reinterpret_cast<Quill &>(std::get<Array>(node.value)[index]);
        }

        if (IsMap()) {
            return reinterpret_cast<Quill &>(std::get<Map>(node.value)[QuillValue{
                .value = static_cast<int>(index),
                .type = ValueType::Integer,
            }]);
        }

        assert(false);
    }

    Quill Quill::Pop() {
        assert(IsArray());
        return Quill{std::get<Array>(node.value).Pop()};
    }

    std::string Quill::ToString(const size_t spaces, const int floatPrecision, const size_t indent) const {
        const std::string tab(indent, ' ');
        const std::string innerTab(indent + spaces, ' ');

        return std::visit([&](const auto &v) -> std::string {
            using T = std::decay_t<decltype(v)>;

            if constexpr (std::is_same_v<T, Struct>) {
                std::string out;
                if (!v.name.empty()) {
                    out += v.name + " ";
                }

                out += "{\n";
                for (const auto &[key, val]: v.fields) {
                    out += innerTab + key + ": " + Quill(val).ToString(spaces, floatPrecision, indent + spaces) + ",\n";
                }

                out += tab + "}";
                return out;
            }

            if constexpr (std::is_same_v<T, Array>) {
                std::string out = "[\n";
                for (const auto &el: v.elements) {
                    out += innerTab + Quill(el).ToString(spaces, floatPrecision, indent + spaces) + ",\n";
                }

                out += tab + "]";
                return out;
            }

            if constexpr (std::is_same_v<T, Map>) {
                if (v.entries.empty()) {
                    return "[:]";
                }

                std::string out = "[\n";
                for (const auto &[k, val]: v.entries) {
                    out += innerTab + Quill(k).ToString(spaces, floatPrecision, indent + spaces) + ": "
                            + Quill(val).ToString(spaces, floatPrecision, indent + spaces) + ",\n";
                }

                out += tab + "]";
                return out;
            }

            if constexpr (std::is_same_v<T, std::string>) { return "\"" + v + "\""; }
            if constexpr (std::is_same_v<T, int64_t>) { return std::to_string(v); }
            if constexpr (std::is_same_v<T, bool>) { return v ? "true" : "false"; }
            if constexpr (std::is_same_v<T, double>) {
                std::ostringstream oss;
                oss << std::setprecision(floatPrecision) << std::showpoint << v;
                return oss.str();
            }

            if constexpr (std::is_same_v<T, std::monostate>) { return "null"; }
        }, node.value);
    }
}
