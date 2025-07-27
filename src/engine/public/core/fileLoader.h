#pragma once

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>

#include <cstring>  // Include for strcpy

// General template function declaration
template <typename T>
T read_file(const std::string& filePath);

// Specialization for std::string
template <>
std::string read_file<std::string>(const std::string& filePath) {
    // Path should be relative to the root of the Emscripten virtual file system
    std::ifstream file(filePath);

    if (!file.is_open()) {
        std::cerr << "Failed to open shader file: " << filePath << std::endl;
        return "";
    }

    std::stringstream buffer;
    buffer << file.rdbuf();  // Read the file's content into the stringstream
    return buffer.str();      // Convert the stream into a string and return it
}

// Specialization for char*
template <>
char* read_file<char*>(const std::string& filePath) {
    // Path should be relative to the root of the Emscripten virtual file system
    std::ifstream file(filePath);

    if (!file.is_open()) {
        std::cerr << "Failed to open shader file: " << filePath << std::endl;
        return nullptr;
    }

    std::stringstream buffer;
    buffer << file.rdbuf();  // Read the file's content into the stringstream
    std::string content = buffer.str();  // Convert the stream into a string

    // Allocate memory for the char* and copy the content
    char* result = new char[content.size() + 1];  // +1 for the null terminator
#ifdef _WIN32
    strcpy_s(result, content.size() + 1, content.c_str());// Copy the string content to the char*

#else
    std::strcpy(result, content.c_str());
#endif  

    return result;  // Caller is responsible for freeing this memory
}
