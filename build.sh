#!/bin/bash

#warnings="-Wno-writable-strings -Wno-format-security -Wno-deprecated-declarations -Wno-switch"
includes="-Ivendors/include -Ivendors/include/glad/include -Ivendors/include/SDL2 -Ivendors -Iimgui -Iimgui/backends -Ivendors/include/assimp"

# Replace Windows-specific libraries with their Linux equivalents
libs="-luser32 -lopengl32 -lvendors/lib/SDL2/SDL2 -lvendors/lib/SDL2/SDL2main -lvendors/lib/assimp/assimp-vc143-mt"

# Build the executable for Linux
clang -x c++ $includes -g src/main.cpp src/shader.cpp imgui/imgui*.cpp imgui/backends/imgui_impl_sdl2.cpp imgui/backends/imgui_impl_opengl3.cpp vendors/include/glad/src/glad.c -o game.exe $libs
