@echo off

:: Path to Premake executable
set PREMAKE5="C:\Users\user\Downloads\premake-5.0.0-beta2-windows\premake5.exe"

:: Path for raddbg
set DBG="C:\Users\user\OneDrive\Desktop\raddg\raddbg.exe"

:: Run Premake to configure the project with Debug settings
%PREMAKE5% --file=premake5.lua vs2022 --cc=clang

:: Set the path to MSBuild (adjust according to your Visual Studio installation)
set MSBUILD="C:\Program Files\Microsoft Visual Studio\2022\Community\MSBuild\Current\Bin\MSBuild.exe"

:: Path to the output executable - adjust the path as needed
set EXECUTABLE_PATH=bin\Debug\MyGame.exe

:: Build the solution
%MSBUILD% "build\MyGame.sln" /p:Configuration=Debug /p:Platform="x64"

:: Check if the build was successful
IF %ERRORLEVEL% NEQ 0 (
    echo Build failed with errors.
) ELSE (
    echo Build succeeded. Running the application...
    %DBG% -d %EXECUTABLE_PATH%"
)

pause
