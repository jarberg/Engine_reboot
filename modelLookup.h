#pragma once

#include <map>
#include <string>
#include <vector>
#include <iostream>
#include <fstream>


struct model {
	std::string name;
	std::vector<float> vertex_pos;

	model() = default;
	model(const std::string& name, const std::vector<float>& vertex_pos)
		: name(name), vertex_pos(vertex_pos) {}
	model(const model& other) = default;
	model(model&& other) noexcept = default;

	model& operator=(const model& other) = default;
	model& operator=(model&& other) noexcept = default;

	~model() = default;

	friend std::ostream& operator <<(std::ostream& os, const model& m) {
		os << m.name << "\n";
		os << m.vertex_pos.size() << "\n";
		for (const auto& v : m.vertex_pos) {
			os << v << " ";
		}
		os << "\n";
		return os;
	}

	// Overload the input stream operator for deserialization
	friend std::istream& operator>>(std::istream& is, model& m) {
		std::getline(is, m.name);
		size_t size;
		is >> size;
		m.vertex_pos.resize(size);
		for (size_t i = 0; i < size; ++i) {
			is >> m.vertex_pos[i];
		}
		is.ignore(std::numeric_limits<std::streamsize>::max(), '\n');  // Skip newline
		return is;
	}
};

class model_datatable {
private:
	void load_dataTable();
	void save_dataTable();
	std::map<int, model> model_datatable_map;
	int generate_new_id();

public:
	model get_model(int id);
	void add_model(model new_model);

	// Constructor that loads the data on initialization
	model_datatable() {
		load_dataTable();
	}
};

