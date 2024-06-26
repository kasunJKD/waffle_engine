@echo off

:: Path to Premake executable
set PREMAKE5="C:\Users\user\Downloads\premake-5.0.0-beta2-windows\premake5.exe"

:: Run Premake to configure the project with Debug settings
%PREMAKE5% --file=premake5.lua vs2022 --cc=clang

:: Set the path to MSBuild (adjust according to your Visual Studio installation)
set MSBUILD="C:\Program Files\Microsoft Visual Studio\2022\Community\MSBuild\Current\Bin\MSBuild.exe"

:: Build the solution
%MSBUILD% "build\MyGame.sln" /p:Configuration=Debug /p:Platform="x64"

:: Check if the build was successful
IF %ERRORLEVEL% NEQ 0 (
    echo Build failed with errors.
)

