cmake_minimum_required(VERSION 3.14)
project(Engine)

# C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# External libraries
set(GLAD_DIR "${CMAKE_SOURCE_DIR}/lib/glad")
set(ENTT_DIR "${CMAKE_SOURCE_DIR}/lib/entt")

# Source and header files
file(GLOB_RECURSE SOURCE_FILES
    "${CMAKE_SOURCE_DIR}/src/engine/private/*.cpp"
)

file(GLOB_RECURSE HEADER_FILES 
    "${CMAKE_SOURCE_DIR}/include/*.h"
    "${CMAKE_SOURCE_DIR}/include/*.hpp"
    "${ENTT_DIR}/*.hpp"
    "${CMAKE_SOURCE_DIR}/src/engine/public/*.h"
)

# Platform-specific files
if(EMSCRIPTEN)
    file(GLOB_RECURSE PLATFORM_SOURCES
        "${CMAKE_SOURCE_DIR}/src/platform/emscripten/*.cpp"
    )
else()
    file(GLOB_RECURSE PLATFORM_SOURCES
        "${CMAKE_SOURCE_DIR}/src/platform/win/*.cpp"
    )
    file(GLOB GLAD_SOURCE "${GLAD_DIR}/include/glad/src/glad.c")
    list(APPEND SOURCE_FILES ${GLAD_SOURCE})
endif()

list(APPEND SOURCE_FILES ${PLATFORM_SOURCES})

# Executable
add_executable(Main ${SOURCE_FILES} ${HEADER_FILES})

# Include paths
target_include_directories(Main PUBLIC
    ${CMAKE_SOURCE_DIR}/include
    ${GLAD_DIR}/include
    ${CMAKE_SOURCE_DIR}/src/engine/public
    ${ENTT_DIR}
)

# Emscripten settings
if(EMSCRIPTEN)
    message(STATUS "Building for Emscripten (WebAssembly)")
    
    set(CMAKE_EXECUTABLE_SUFFIX ".html")
    set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/output)

    set_target_properties(Main PROPERTIES
        OUTPUT_NAME "output"
        RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/output
    )

    # Preload Resources folder if it exists
    if(EXISTS "${CMAKE_SOURCE_DIR}/Resources")
        message(STATUS "Adding Resources folder to virtual filesystem")
        set_target_properties(Main PROPERTIES
            LINK_FLAGS "-s USE_WEBGL2=1 -s FULL_ES3=1 --preload-file ${CMAKE_SOURCE_DIR}/Resources@Resources"
        )
    else()
        message(WARNING "Resources folder not found — skipping preload.")
        set_target_properties(Main PROPERTIES
            LINK_FLAGS "-s USE_WEBGL2=1 -s FULL_ES3=1"
        )
    endif()
else()
    message(STATUS "Building native executable")
endif()
