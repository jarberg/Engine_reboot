@echo off
setlocal enabledelayedexpansion

:: Quiet the EMSDK environment setup
set EMSDK_QUIET=1
call "C:/Users/mjarb/Projects/emsdk/emsdk_env.bat"

:: Verify EMSDK environment variables
echo EMSDK Path: %EMSDK%
echo Emscripten Path: %EMSCRIPTEN%

:: Create build directory if it doesn't exist
if not exist build mkdir build
cd build

:: Configure CMake for Emscripten and build using Ninja
cmake -G "Ninja" ^
      -DCMAKE_TOOLCHAIN_FILE="C:/Users/mjarb/Projects/emsdk/upstream/emscripten/cmake/Modules/Platform/Emscripten.cmake" ^
      -DCMAKE_BUILD_TYPE=Release ..

:: Check if CMake configuration was successful
if errorlevel 1 (
    echo CMake configuration failed.
    cd ..
    pause
    exit /b 1
)

:: Build the project
cmake --build .

:: Check if the build was successful
if errorlevel 1 (
    echo Build failed.
    cd ..
    pause
    exit /b 1
)

cd ..
echo Build completed successfully.
pause
