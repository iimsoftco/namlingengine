# Namling Engine v1.2
A 2D game engine based on the memes of "Namlings".
## Features:
### Maze and Collect modes
### Switch from play mode to editor by clicking tab
### .nam file Level saving (enter key while in editor or play mode)
### NAM-SU

# Info
The binary on this repo is for Linux, but the source code is included too so you can compile it to other platforms.

# License
You can redistribute, modify and do basically anything but don't claim this engine as yours: I put a lot of work into it!

# Compile command
g++ namlingengine.cpp -o engine \
    -lsfml-graphics -lsfml-window -lsfml-system

# Namlinx Language Documentation

Namlinx is a simple, lightweight scripting language designed specifically for customizing behaviors in the Namling Engine. It enables control over game objects, like player movement, using straightforward syntax and a small set of commands.

Namlinx scripts are plain text files (with .namx extension) that the engine loads and executes each frame, allowing dynamic control over variables like position and speed based on input conditions.

# Key Concepts
Variables: Namlinx supports a few predefined floating-point variables:

x — horizontal position (e.g., player X coordinate)

y — vertical position (e.g., player Y coordinate)

speed — movement speed scalar

Input Checking: You can check if a key is pressed via isKeyDown("KEY"), where KEY is one of "W", "A", "S", "D" (or any supported key name).

Time Delta: A special variable dt (delta time in seconds) is available to ensure smooth frame-rate-independent movement.

# Syntax
Variable Assignment
Set a variable’s value:


speed = 150
Conditional Movement
Modify variables conditionally based on key input:


if isKeyDown("W") then y = y - speed * dt
if isKeyDown("S") then y = y + speed * dt
if isKeyDown("A") then x = x - speed * dt
if isKeyDown("D") then x = x + speed * dt
This moves the object up/down/left/right while the respective keys are pressed, scaled by speed and delta time.

# Supported Statements
Assignment: <variable> = <value>
Assign a constant value or expression to a variable. Currently only speed = <number> is supported as a direct assignment.

Conditional Update:


if isKeyDown("KEY") then <var> = <var> +/- speed * dt
This modifies x or y based on whether a key is pressed, incrementing or decrementing by speed * dt.

# Limitations
Only variables x, y, and speed are supported.

Only simple arithmetic expressions of the form <var> = <var> +/- speed * dt inside if isKeyDown() conditions are recognized.

No loops, functions, or complex expressions.

Keys must be provided as uppercase strings "W", "A", "S", "D".

## Example Script: Basic WASD Movement
speed = 150
if isKeyDown("W") then y = y - speed * dt
if isKeyDown("S") then y = y + speed * dt
if isKeyDown("A") then x = x - speed * dt
if isKeyDown("D") then x = x + speed * dt
How to Use
Write your custom behavior in a behavior.namx text file using the supported syntax.

Place the .namx file inside the assets/ folder of your project.

The engine will load and run the script each frame, updating the controlled object's position accordingly.

Modify speed to adjust movement speed.

Use conditional statements to react to key presses.
