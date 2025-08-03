#pragma once

#include <iostream>
#include <fstream>
#include <string>
#include <cstring>

// General template function declaration
template <typename T>
T read_file(const std::string& filePath);

// Specialization for std::string
template <>
std::string read_file<std::string>(const std::string& filePath) {
    std::ifstream file(filePath, std::ios::binary | std::ios::ate);  // Open at end

    if (!file.is_open()) {
        std::cerr << "Failed to open shader file: " << filePath << std::endl;
        return "";
    }

    std::streamsize size = file.tellg();           // Get file size
    file.seekg(0, std::ios::beg);                  // Seek to beginning

    std::string buffer(size, '\0');
    file.read(&buffer[0], size);                   // Read exactly 'size' bytes
    return buffer;
}

// Specialization for char*
template <>
char* read_file<char*>(const std::string& filePath) {
    std::ifstream file(filePath, std::ios::binary | std::ios::ate);

    if (!file.is_open()) {
        std::cerr << "Failed to open shader file: " << filePath << std::endl;
        return nullptr;
    }

    std::streamsize size = file.tellg();
    file.seekg(0, std::ios::beg);

    std::string content(size, '\0');
    file.read(&content[0], size);

    char* result = new char[content.size() + 1];
#ifdef _WIN32
    strcpy_s(result, content.size() + 1, content.c_str());
#else
    std::strcpy(result, content.c_str());
#endif

    return result;
}
