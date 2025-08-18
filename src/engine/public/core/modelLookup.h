#pragma once

#include <map>
#include <string>
#include <vector>
#include <iostream>
#include <fstream>
#include <filesystem>

#include <json.hpp>

#include "core/singleton.h"
#include "core/types.h"



class model_datatable:public Singleton<model_datatable> {
    friend class Singleton<model_datatable>;
private:
    model_datatable() {
        load_dataTable();
    };
	void save_dataTable();
	std::unordered_map<std::string, int> modelPath_datatable_map;
	std::unordered_map<int, std::shared_ptr<Model>> model_datatable_map;
	int generate_new_id();

public:

	std::shared_ptr<Model> get_model(int id);
	void print_data() {
		for (const auto& [id, model] : model_datatable_map) {
			std::cout << "ID: " << id << ", Name: " << model->name << std::endl;
			std::cout << "Vertices: ";
			for (float v : model->vertex_pos) {
				std::cout << v << " ";
			}
			std::cout << std::endl << "UVs: ";
			for (float v : model->uvs) {
				std::cout << v << " ";
			}
			std::cout << std::endl;
		}
	}

	std::unordered_map<int, std::shared_ptr<Model>>& get_model_map() {
		return model_datatable_map;
	}

	void add_model(std::shared_ptr<Model> new_model);
	void add_model(std::string _name, std::vector<float> _points, std::vector<unsigned short> _indices, std::vector<float> _uvs);
	
	// Constructor that loads the data on initialization
	void load_dataTable();

	std::shared_ptr<Model> error_model();

	~model_datatable() {
		save_dataTable();
	}
};

