@echo off

:: Set up paths
set PREMAKE5="C:\Users\user\Downloads\premake-5.0.0-beta2-windows\premake5.exe"
set MSBUILD="C:\Program Files\Microsoft Visual Studio\2022\Community\MSBuild\Current\Bin\MSBuild.exe"
set SDL_DLL_PATH="vendors\lib\SDL2\SDL2.dll"
set ASSIMP_DLL_PATH="vendors\lib\assimp\assimp.dll"
set OUTPUT_DIR="bin\Debug\Sandbox"
set SANDBOX_EXE="%OUTPUT_DIR%\Sandbox.exe"

:: Step 1: Run Premake to generate the Visual Studio solution
if not exist %PREMAKE5% (
    echo Premake executable not found at %PREMAKE5%.
    exit /b 1
)
%PREMAKE5% --file=premake5.lua vs2022
if %ERRORLEVEL% NEQ 0 (
    echo Premake configuration failed.
    exit /b %ERRORLEVEL%
)

:: Step 2: Build the solution with MSBuild
if not exist %MSBUILD% (
    echo MSBuild executable not found at %MSBUILD%.
    exit /b 1
)
%MSBUILD% "build\Engine.sln" /p:Configuration=Debug /p:Platform="x64"
if %ERRORLEVEL% NEQ 0 (
    echo Build failed with errors.
    exit /b %ERRORLEVEL%
) else (
    echo Build succeeded.
)

:: Step 3: Copy required DLLs to the output directory
if exist %SDL_DLL_PATH% (
    copy %SDL_DLL_PATH% %OUTPUT_DIR%
    echo SDL2.dll copied to %OUTPUT_DIR%.
) else (
    echo SDL2.dll not found at %SDL_DLL_PATH%.
    exit /b 1
)

if exist %ASSIMP_DLL_PATH% (
    copy %ASSIMP_DLL_PATH% %OUTPUT_DIR%
    echo assimp.dll copied to %OUTPUT_DIR%.
) else (
    echo assimp.dll not found at %ASSIMP_DLL_PATH%.
    exit /b 1
)

:: Step 4: Run the Sandbox executable
if exist %SANDBOX_EXE% (
    echo Running Sandbox...
    %SANDBOX_EXE%
) else (
    echo Sandbox executable not found at %SANDBOX_EXE%.
    exit /b 1
)
