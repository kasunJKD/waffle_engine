@echo off

:: Path to Premake executable
set PREMAKE5="C:\Users\user\Downloads\premake-5.0.0-beta2-windows\premake5.exe"

:: Check if Premake exists
if not exist %PREMAKE5% (
    echo Premake executable not found at %PREMAKE5%.
    exit /b 1
)

:: Run Premake to configure the project with Debug settings
%PREMAKE5% --file=premake5.lua vs2022
if %ERRORLEVEL% NEQ 0 (
    echo Premake configuration failed.
    exit /b %ERRORLEVEL%
)

:: Set the path to MSBuild (adjust according to your Visual Studio installation)
set MSBUILD="C:\Program Files\Microsoft Visual Studio\2022\Community\MSBuild\Current\Bin\MSBuild.exe"

:: Check if MSBuild exists
if not exist %MSBUILD% (
    echo MSBuild executable not found at %MSBUILD%.
    exit /b 1
)

:: Build the solution
%MSBUILD% "build\Engine.sln" /p:Configuration=Debug /p:Platform="x64"
if %ERRORLEVEL% NEQ 0 (
    echo Build failed with errors.
    exit /b %ERRORLEVEL%
) else (
    echo Build succeeded.
)

