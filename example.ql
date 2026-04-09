// Example Quill file
/* Block comment */
Entity {
    position: {
        x: 1.0,
        y: 2.0,
        z: 3.0
    },
    color: {
        r: 255,
        g: 0,
        b: 128,
        a: 255
    },
    rotation: {
        x: 0.0,
        y: 0.0,
        z: 0.0,
        w: 1.0
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
