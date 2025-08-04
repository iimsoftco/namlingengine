

![GitHub Repo stars](https://img.shields.io/github/stars/iimsoftco/namlingengine)

# Namling Engine v2.3
A 2D game engine based on the memes of "Namlings".
## Features:
### Namlinx coding support - make your own game easily and fast!

# Info
The binary on this repo is for Linux, but the source code is included too so you can compile it to other platforms.

# License
You can redistribute, modify and do basically anything but don't claim this engine as yours: I put a lot of work into it!

# Compile command
g++ namlingengine.cpp -o engine \
    -lsfml-graphics -lsfml-window -lsfml-system


# Namlinx Documentation

## Overview

Namlinx is a simple scripting language designed to control game logic, variables, and interactions within the **Namling Engine**.

---

## Syntax and Features

### Variable Assignment

Assign values to variables using `=`:

x = 100
speed = 5 * dt



Variables hold numeric values.

---

### Conditional Statements

Use equality checks with `==` and `then` to run conditional logic:

box_collided == 1 then speed = 0



---

### Arithmetic Expressions

Supports basic arithmetic operators: `+`, `-`, `*`, `/`, `^`

Examples:

distance = speed * dt
angle = sin(angle_rad)



Built-in functions: `sin()`, `cos()`, `random()`

---

### Supported Variables

- `dt` — Delta time (time elapsed per frame)
- `random()` — Generates a random float between 0 and 1

---

### Game-related Variables

- `box_collided`, `than_collided`, `gim_collided` — Collision flags set by the Namling Engine (0 or 1)
- `box_count`, `than_count`, `gim_count` — Number of remaining objects in the game

---

### Game Objects Variables

Define positions and sizes of objects:

- Boxes:
box0_x = 100
box0_y = 100
box0_w = 50
box0_h = 50


- Thanlings:
than0_x = 300
than0_y = 200


- Gimbaps:
gim0_x = 500
gim0_y = 400



---

### Example Script

box0_x = 100
box0_y = 100
box0_w = 50
box0_h = 50
box_count = 1

than0_x = 300
than0_y = 200
than_count = 1

gim0_x = 500
gim0_y = 400
gim_count = 1

box_collided == 1 then box_count = box_count - 1
than_collided == 1 then than_count = than_count - 1
gim_collided == 1 then gim_count = gim_count - 1





---

## Notes

- Each command should be on its own line.
- Variables are dynamically created when assigned.
- The Namling Engine runs the interpreter every frame, updating variables and logic.
- Collision flags and object counts are managed by the Namling Engine and exposed to the script.

---

## Summary

Namlinx provides simple scripting to handle game logic with variables, conditionals, arithmetic, and built-in functions, allowing easy interaction with game objects (boxes, thanlings, gimbaps) and states within the Namling Engine.
A script.namx should be placed inside the assets/ folder.
