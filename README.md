# Namling Engine v1.3
A 2D game engine based on the memes of "Namlings".
## Features:
### Namlinx coding support - make your own game!

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
Assignment: <variable> = <value>
Assign a constant value or expression to a variable.





Place a script.namx file inside the assets/ folder of your project.

The engine will load and run the script each frame, updating the controlled object's position accordingly.

Modify speed to adjust movement speed.

Use conditional statements to react to key presses.
