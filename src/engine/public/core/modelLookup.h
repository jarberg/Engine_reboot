#pragma once

#include <map>
#include <string>
#include <vector>
#include <iostream>
#include <fstream>
#include <filesystem>

#include "core/singleton.h"
#include "core/types.h"



class model_datatable:public Singleton<model_datatable> {
    friend class Singleton<model_datatable>;
private:
    model_datatable() {
        load_dataTable();
    };
	void save_dataTable();
	std::map<unsigned int, model> model_datatable_map;
	int generate_new_id();

public:

	model get_model(int id);

	std::map<unsigned int, model>& get_model_map() {
		return model_datatable_map;
	}

	void add_model(model new_model); 
	void add_model(std::string, std::vector<float> points);
	
	// Constructor that loads the data on initialization
	void load_dataTable();

	~model_datatable() {
		save_dataTable();
	}
};

