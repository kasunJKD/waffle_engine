@echo off
setlocal

REM --- Set compilers to Clang (if not already in your PATH) ---
set CC=clang
set CXX=clang++

REM --- Create the build directory if it doesn't exist ---
if not exist build (
    mkdir build
)

REM --- Change to the build directory ---
cd build

REM --- Run CMake with the Ninja generator and export compile commands ---
cmake -G Ninja -DCMAKE_EXPORT_COMPILE_COMMANDS=ON ..

if errorlevel 1 (
    echo CMake configuration failed.
    pause
    exit /b 1
)

REM --- Build the project using Ninja ---
ninja

if errorlevel 1 (
    echo Build failed.
    pause
    exit /b 1
)

REM --- Change to the output directory ---
cd bin\Debug

if exist Sandbox.exe (
    echo Running Sandbox.exe...
    cd /d %~dp0
    D:\Personal\waffle_2\waffle_engine\build\bin\Debug\Sandbox.exe
    
) else (
    echo Sandbox.exe not found. Please check your build configuration.
    pause
)

endlocal
