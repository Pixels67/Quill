# Quill
Data serialization format meant for [Flock](https://github.com/Pixels67/Flock).

### Why?
your mom

### How does it work?
Hashmap keys can be anything as opposed to just strings and there are no
unnecessary quotation marks everywhere. Trailing commas and comments are supported.
Structs can be typed or untyped.  
  
File extension should be QL or QUILL.
```c++
// Example Quill file
/* Block comment */
Entity {
    position: {
        x: 1.0,
        y: 2.0,
        z: 3.0,
    },
    color: {
        r: 255,
        g: 0,
        b: 128,
        a: 255,
    },
    rotation: {
        x: 0.0,
        y: 0.0,
        z: 0.0,
        w: 1.0,
    },

    health: HealthComponent {
        current_hp: 100.0,
        max_hp: 200.0,
    },

    boolean: false,
    nullable: null,
    string: "hello world",
    escaped: "escape:\\ \"test\"",

    tags: ["enemy", "boss", "undead"],

    weights: [
        4.6: 5,
        3.14: 3,
    ],

    empty_map: [:],
}
```

Integers can have the following formats:
```js
+99, 42, 0, -17
0xAF, 0o37, 0b1011
```
Floats can have the following formats:
```js
+1.0, 3.1415, -0.01
5e+22, 1e06, -2E-2
6.626e-34
inf, +inf, -inf
nan, +nan, -nan
```
All strings are multi-line strings, though escape codes must be used regardless.
Valid escape characters are `//`, `/"`, and `/n`.

### Library Usage
```c++
#include <iostream>

#include "Quill.hpp"

int main() {
    using namespace Ql;

    // Serialization
    Quill val;
    val["number"] = 500.0;
    val["bool"] = false;

    val["obj"] = Struct{};
    val["obj"]["happy"] = true;

    val["arr"] = Array{};
    val["arr"].Push(4);
    val["arr"].Push(2);

    val["map"] = Map{};
    val["map"]["A"] = "Automatic";
    val["map"]["B"] = "Bear";

    val["null"] = Null{};

    std::cout << val.ToString() << '\n';

    // Deserialization
    std::string src = R"({
        health: HealthComponent {
            current_hp: 100.0,
            max_hp: 200.0,
        },
    })";
    
    val = Quill::Parse(src);
    val["health"]["f"] = 50.0f;
    assert(val["health"]["current_hp"].As<float>() == 100.0f);
    assert(val["health"]["f"].As<float>() == 50.0f);
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
```
