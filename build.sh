#!/bin/bash

warnings="-Wno-writable-strings -Wno-format-security -Wno-deprecated-declarations -Wno-switch"
includes="-Ithird_party -Ithird_party/Include"

# Replace Windows-specific libraries with their Linux equivalents
libs="-lX11 -lGL -lGLU"

# Build the executable for Linux
clang++ $includes -g src/main.cpp -o engine $libs $warnings

