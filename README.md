

![GitHub Repo stars](https://img.shields.io/github/stars/iimsoftco/namlingengine)

# Namling Engine v2.2ex
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

Namlinx is a simple scripting language designed for basic game logic integration in Namling Engine. It supports variable assignments, conditional statements, object creation, and simple math operations.

---

## Script Structure

- Scripts are .namx files.
- Lines starting with `//` or `-` are comments and ignored.
- Each statement or command is written on its own line.
- Supports variable assignment, conditional execution, and commands to create game objects.

---

## Variables

- Two types of variables:
  - **Numeric variables**: Assigned with `=`, e.g., `x=10`.
  - **String variables**: Assigned with quotes, e.g., `text="Hello"`.

- Variables can be used in expressions and conditions.

---

## Expressions

- Supports arithmetic operations: `+`, `-`, `*`, `/`, `^` (power).
- Supports math functions: `sin()`, `cos()`.
- Supports constant variables such as `dt` (delta time) and `random()` (random float between 0 and 1).
- Variables can be combined to form expressions, e.g., `x = 5 + 3 * random()`.

---

## Commands

### Object Creation

- `box x y size color`  
  Creates a rectangular box at `(x, y)` with given `size` and `color`.

- `than x y`  
  Creates a thanling sprite at `(x, y)`.

- `gim x y`  
  Creates a gimbap sprite at `(x, y)`.

---

## Conditional Statements

- Syntax:  
  `variable == value then <statement>`  
  Executes `<statement>` if the variable equals the value.

- Supports simple equality checks for control flow.

---

## Predefined Variables (from C++ context)

- `box_collided` (0 or 1) — Set when player collides with any box.
- `than_collided` (0 or 1) — Set when player collides with any thanling.
- `gim_collided` (0 or 1) — Set when player collides with any gimbap.
- `box_count` (number) — Current number of boxes remaining.
- `than_count` (number) — Current number of thanlings remaining.
- `gim_count` (number) — Current number of gimbaps remaining.
- `dt` — Delta time, useful for frame-based calculations.
- `random()` — Returns a random float between 0 and 1.

---

## Example Script

A documented Namlinx script is included in assets/ folder.
