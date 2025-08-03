#pragma once

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <cstring>

// General template function declaration
template <typename T>
T read_file(const std::string& filePath);

// Specialization for std::string
template <>
std::string read_file<std::string>(const std::string& filePath) {
    std::ifstream file(filePath, std::ios::in | std::ios::binary);  // <-- BINARY MODE

    if (!file.is_open()) {
        std::cerr << "Failed to open shader file: " << filePath << std::endl;
        return "";
    }

    std::stringstream buffer;
    buffer << file.rdbuf();
    return buffer.str();
}

// Specialization for char*
template <>
char* read_file<char*>(const std::string& filePath) {
    std::ifstream file(filePath, std::ios::in | std::ios::binary);  // <-- BINARY MODE

    if (!file.is_open()) {
        std::cerr << "Failed to open shader file: " << filePath << std::endl;
        return nullptr;
    }

    std::stringstream buffer;
    buffer << file.rdbuf();
    std::string content = buffer.str();

    char* result = new char[content.size() + 1];
#ifdef _WIN32
    strcpy_s(result, content.size() + 1, content.c_str());
#else
    std::strcpy(result, content.c_str());
#endif

    return result;
}
