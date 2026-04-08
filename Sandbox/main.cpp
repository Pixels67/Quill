#include "Quill.hpp"

int main() {
    using namespace Ql;

    std::string src = R"({
        health: HealthComponent {
            current_hp: 100.0,
            max_hp: 200.0,
        },
    })";

    Quill val = Quill::Parse(src);
    assert(val["health"]["current_hp"].As<float>() == 100.0f);
    assert(val["health"].TypeName() == "HealthComponent");

    src = R"({ tags: ["enemy", "boss", "undead"] })";
    val = Quill::Parse(src);
    assert(val["tags"][0].As<std::string>() == "enemy");
    assert(val["tags"][2].As<std::string>() == "undead");

    src = R"({ tags: [1: "enemy", 5: "boss", 4: "undead"] })";
    val = Quill::Parse(src);
    assert(val["tags"][1].As<std::string>() == "enemy");
    assert(val["tags"][4].As<std::string>() == "undead");
}