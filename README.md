![Static Badge](https://img.shields.io/badge/Version-2_._1-blue)

# Namling Engine v2.1
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

Namlinx is a simple, lightweight scripting language designed specifically for customizing behaviors in the Namling Engine. It enables control over game objects, like player movement, using straightforward syntax and a small set of commands.

Namlinx scripts are plain text files (with .namx extension) that the engine loads and executes each frame, allowing dynamic control over variables like position and speed based on input conditions.

# Key Concepts
Variables: Namlinx supports a few predefined floating-point variables:

x — horizontal position (e.g., player X coordinate)

y — vertical position (e.g., player Y coordinate)


# Syntax
Variable Assignment
Set a variable’s value:


speed = 150


# Supported Statements
## Assignment: 
<variable> = <value>
Assign a constant value or expression to a variable.
## Randomize:
random()
## Win condition:
win
## Text:
text





Place a script.namx file inside the assets/ folder of your project. (explained and documented game script included)

The engine will load and run the script each frame, updating the controlled object's position accordingly.
