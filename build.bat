@echo off
echo === Locate Visual Studio ===

set "VSWHERE=%ProgramFiles(x86)%\Microsoft Visual Studio\Installer\vswhere.exe"

REM Find vcvarsall.bat
for /f "usebackq tokens=*" %%i in (`"%VSWHERE%" -latest -property installationPath`) do (
    set "VS_PATH=%%i"
)

if not defined VS_PATH (
    echo ERROR: Could not find Visual Studio installation.
    exit /b 1
)

REM Initialize the 64-bit compiler environment
call "%VS_PATH%\VC\Auxiliary\Build\vcvarsall.bat" x64

if errorlevel 1 (
    echo ERROR: vcvarsall.bat failed.
    exit /b 1
)
echo === Build ===

set "ROOT=%~dp0"

REM --- Paths: adjust these to wherever you installed each lib ---
set "SDL2_DIR=%ROOT%vendors\lib\SDL2"
set "SDL2_INCLUDE=%ROOT%vendors\include\SDL2"
set "GLAD_DIR=%ROOT%vendors\include\glad"
set "IMGUI_DIR=%ROOT%vendors\imgui"

set CommonCompilerFlags=-MT -nologo -Gm- -GR- -EHac- -Od -Oi -WX -W4 ^
    -wd4201 -wd4100 -wd4189 -wd4127 -wd4505 -FC -Z7 ^
    -I"%SDL2_INCLUDE%" ^
    -I"%GLAD_DIR%\include" ^
    -I"%IMGUI_DIR%" ^
    -I"%IMGUI_DIR%\backends"

set CommonLinkerFlags=-incremental:no -opt:ref ^
    user32.lib gdi32.lib winmm.lib shell32.lib opengl32.lib ^
    "%SDL2_DIR%\SDL2.lib" ^
    "%SDL2_DIR%\SDL2main.lib"

IF NOT EXIST .\build mkdir .\build
pushd .\build

REM --- Compile third-party translation units (once each, rarely change) ---

REM GLAD
cl -MT -nologo -W0 -c -I"%GLAD_DIR%\include" "%GLAD_DIR%\src\glad.c"

REM Dear ImGui core
cl -MT -nologo -W0 ^
    -I"%IMGUI_DIR%" ^
    -c "%IMGUI_DIR%\imgui.cpp" ^
       "%IMGUI_DIR%\imgui_draw.cpp" ^
       "%IMGUI_DIR%\imgui_tables.cpp" ^
       "%IMGUI_DIR%\imgui_widgets.cpp"

REM Dear ImGui backends (SDL2 + OpenGL3)
cl -MT -nologo -W0 ^
    -I"%IMGUI_DIR%" ^
    -I"%IMGUI_DIR%\backends" ^
    -I"%SDL2_INCLUDE%" ^
    -I"%GLAD_DIR%\include" ^
    -c "%IMGUI_DIR%\backends\imgui_impl_sdl2.cpp" ^
       "%IMGUI_DIR%\backends\imgui_impl_opengl3.cpp"

REM --- Compile your app and link everything ---
cl %CommonCompilerFlags% %ROOT%src\platform.cpp ^
    glad.obj ^
    imgui.obj imgui_draw.obj imgui_tables.obj imgui_widgets.obj ^
    imgui_impl_sdl2.obj imgui_impl_opengl3.obj ^
    /link %CommonLinkerFlags%

REM Copy SDL2.dll next to the exe
copy /Y "%SDL2_DIR%\SDL2.dll" "SDL2.dll" >nul

popd
