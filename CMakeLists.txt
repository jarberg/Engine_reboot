cmake_minimum_required(VERSION 3.14)
project(Engine)

# Set the C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)



# Path to the GLAD directory
set(GLAD_DIR "${CMAKE_SOURCE_DIR}/lib/glad")
set(ENTT_DIR "${CMAKE_SOURCE_DIR}/lib/entt")

# Define source and header files
file(GLOB_RECURSE SOURCE_FILES 
    "${CMAKE_SOURCE_DIR}/src/*.cpp"
)

file(GLOB_RECURSE HEADER_FILES 
     "${CMAKE_SOURCE_DIR}/include/*.h"
     "${CMAKE_SOURCE_DIR}/include/*.hpp" 
     "${ENTT_DIR}/*.hpp"
     "${CMAKE_SOURCE_DIR}/src/*.h"

)

# Add the GLAD source file only for non-Emscripten builds
if(NOT EMSCRIPTEN)
    file(GLOB GLAD_SOURCE "${GLAD_DIR}/include/glad/src/glad.c")
    list(APPEND SOURCE_FILES ${GLAD_SOURCE})
endif()

# Add the executable
add_executable(Main ${SOURCE_FILES} ${HEADER_FILES})

# Include directories# Include directories
target_include_directories(Main PUBLIC
    ${CMAKE_SOURCE_DIR}/include   # Your public headers
    ${GLAD_DIR}/include           # GLAD headers
    ${CMAKE_SOURCE_DIR}/src               # project headers
    ${ENTT_DIR}
)

# Detect if we're using Emscripten
if(EMSCRIPTEN)
    message("Using emscripten")

    # Emscripten specific settings
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}")
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} ")
    # Custom target for building WebAssembly
    set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/output)
    add_custom_target(build_wasm ALL
        COMMAND emcc ${SOURCE_FILES}
        -I ${CMAKE_SOURCE_DIR}/include
 	-I ${CMAKE_SOURCE_DIR}/src  
	-I ${ENTT_DIR}
        ${CMAKE_CXX_FLAGS} -s USE_WEBGL2=1 -s FULL_ES3=1 --preload-file "Resources"
        -o ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/output.html
        DEPENDS ${SOURCE_FILES} ${HEADER_FILES}
        WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
        COMMENT "Building WebAssembly project with Emscripten"
    )
else()
    message("Not using emscripten")
    # Additional settings for non-Emscripten builds (e.g., MSVC) go here
endif()
