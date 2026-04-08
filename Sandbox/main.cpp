#include <array>
#include <iostream>
#include <vector>

#include "Quill.hpp"

std::string data = R"(
{
    position: xyz(1.0, 2.0, 3.0),
    tint: rgba(255, 0, 128, 255),
    state: State::Idle,
    boolean: false,
    nullable: null,
    string: ""
}
)";

int main() {
    using namespace Ql;

    const Quill quill = Quill::Parse(data);

    bool correct = quill["position"].As<std::array<float, 3> >()[0] == 1.0;
    assert(correct);

    correct = quill["tint"].As<std::array<int, 4> >()[0] == 255;
    assert(correct);

    correct = quill["boolean"].As<bool>() == false;
    assert(correct);

    correct = quill["nullable"].IsNull();
    assert(correct);

    correct = quill["string"].As<std::string>().empty();
    assert(correct);

    std::string src = R"(
{
    health: HealthComponent {
        current_hp: 100.0,
        max_hp: 200.0,
    },
}
)";

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
