#!/bin/bash

#warnings="-Wno-writable-strings -Wno-format-security -Wno-deprecated-declarations -Wno-switch"
includes="-Ivendors/include"

# Replace Windows-specific libraries with their Linux equivalents
libs="-luser32 -lopengl32 -lvendors/lib/SDL2/SDL2 -lvendors/lib/SDL2/SDL2main"

# Build the executable for Linux
clang++ $includes -g src/main.cpp -o game.exe $libs
