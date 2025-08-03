@echo off
setlocal enabledelayedexpansion

:: Setup Emscripten
set EMSDK_QUIET=1
call "C:/Users/mjarb/Projects/emsdk/emsdk_env.bat"

:: Check EMSDK
echo EMSDK Path: %EMSDK%
echo Emscripten Path: %EMSCRIPTEN%

:: Clean previous build
echo Cleaning previous build...
if exist build rmdir /s /q build
if exist output rmdir /s /q output

:: Create build directory
mkdir build
cd build

:: Configure with CMake + Ninja
cmake -G "Ninja" ^
      -DCMAKE_TOOLCHAIN_FILE="C:/Users/mjarb/Projects/emsdk/upstream/emscripten/cmake/Modules/Platform/Emscripten.cmake" ^
      -DCMAKE_BUILD_TYPE=Release ..

if errorlevel 1 (
    echo [ERROR] CMake configuration failed.
    cd ..
    pause
    exit /b 1
)

:: Build
cmake --build .

if errorlevel 1 (
    echo [ERROR] Build failed.
    cd ..
    pause
    exit /b 1
)

cd ..

:: Check output.data
if exist output\output.data (
    echo output.data exists. Size:
    for %%I in (output\output.data) do echo %%~zI bytes
) else (
    echo [WARNING] output.data not generated.
)

echo Build completed successfully.
pause
