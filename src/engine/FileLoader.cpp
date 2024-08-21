#include <iostream>
#include <fstream>
#include <string>


std::string read_file22(std::string filePath) {
	std::string ret = std::string();
	
	std::ifstream file(filePath);

	if(!file.is_open()){
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

