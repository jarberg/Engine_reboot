#pragma once
#include <iostream>
#include <fstream>
#include <string>
#include <cstring>

template <typename T>
T read_file(const std::string& filePath);

// Specialization for std::string
template <>
std::string read_file<std::string>(const std::string& filePath) {
	std::string ret = std::string();
    std::cout << "filepath : " << filePath << std::endl;
	std::ifstream file(filePath);

	if (!file.is_open()) {
		std::cerr << "Failed to open file" << std::endl;
	}

	std::string line;
	while (std::getline(file, line)) {
		ret += line;
		std::cout << line << std::endl;
	}
	file.close();
	std::cout << ret << std::endl;
	return ret;
}

template <>
char* read_file<char*>(const std::string& filePath) {
    std::string ret;  // Use std::string to build the content
    std::cout << "filepath : " << filePath << std::endl;
    std::ifstream file(filePath);
    if (!file.is_open()) {
        std::cerr << "Failed to open file: " << filePath << std::endl;
        return nullptr;
    }

    std::string line;
    while (std::getline(file, line)) {
        ret += line + '\n';  // Add newline to preserve the file format
    }
    file.close();

    // Allocate memory for the char* and copy the string content
    char* result = new char[ret.size() + 1];  // +1 for null terminator
    strcpy_s(result, ret.size() + 1, ret.c_str());

    return result;  // Caller is responsible for freeing this memory
}