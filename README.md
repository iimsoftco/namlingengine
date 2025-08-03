

![GitHub Repo stars](https://img.shields.io/github/stars/iimsoftco/namlingengine)

# Namling Engine v2.1beta
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

# Namlinx Language Documentation

## 📦 Box Creation

box x y size color
// Creates a box at position (x, y) with given size and color.
// Colors: red, green, blue, etc.
box 100 200 40 red

---

## 🧍 Thanling Placement

than x y
// Places a "thanling" sprite at position (x, y).
than 300 300

---

## 🧾 Text Display

text = "string"
// Displays a string of text on screen.

text_x = number
text_y = number
// Sets the text's position.

text = "YOU WIN"
text_x = 250
text_y = 50

---

## 🧍‍♂️ Player Positioning

x = number
y = number
// Moves the player to position (x, y)

x = 400
y = 300

---

## 🔁 Conditional Execution

if a == b then command
// Runs the command only if the condition is true.

if box_count == 0 then text = "Victory!"

---

## 🔢 Math & Expressions

// Operators:
+  addition
-  subtraction
*  multiplication
/  division
^  exponent

// Functions:
cos(expr)
sin(expr)

// Constants:
dt         // frame time (1/60)
random()   // random float 0–1

angle = angle + dt * 2
x = cos(angle) * 100 + 400

---

## 📦 Box Utilities

box_count
// Number of boxes currently active.

if box_count == 0 then win = 1

---

## 🤖 Thanling Interaction

than_collided
// Is 1 when the player touches any thanling this frame.

if than_collided == 1 then touched = touched + 1

# Run the code

Place a script.namx inside assets/
