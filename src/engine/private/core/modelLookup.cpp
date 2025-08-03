#include <core/modelLookup.h>

model model_datatable::get_model(int id)
{	
    auto m = this->model_datatable_map;
    std::map<unsigned int, model>::iterator ret_iterator;

    if ((ret_iterator = m.find(id)) != m.end()) {
        //if the element is found before the end of the map
        // std::cout << " : found : Value : " << m[id].name << std::endl;
        //if the element is present then you can access it using the index
        return ret_iterator->second;
    }
    return model(std::string("error"), { { 1.0f, 0.0f, -1.0f } });
}

void model_datatable::add_model(model new_model)
{
    int new_id = generate_new_id();
    this->model_datatable_map.emplace(new_id, std::move(new_model));
    save_dataTable();
}

void model_datatable::add_model(std::string _name, std::vector<float> _points)
{
    model newModel(_name, _points);
    add_model(newModel);
}


int model_datatable::generate_new_id() {
    if (model_datatable_map.empty()) {
        return 1; // Start with ID 1 if map is empty
    }
    return model_datatable_map.rbegin()->first + 1; // Generate ID as max key + 1
}

void model_datatable::save_dataTable() {
    std::ofstream file("Resources/models.dat");
    if (file.is_open()) {
        file << "{ \"models\": {";
        for (auto it = model_datatable_map.begin(); it != model_datatable_map.end(); ++it) {
            if (it != model_datatable_map.begin()) {
                file << ", ";
            }
            file << "\"" << it->first << "\": " << it->second;
        }
        file << "} }";
        file.close();
    }
}

void model_datatable::load_dataTable() {
    std::ifstream file("Resources/models.dat");

    if (file.is_open() && file.peek() != std::ifstream::traits_type::eof()) {
        std::string line;
        std::getline(file, line, '{');  // Skip the opening brace
        std::getline(file, line, '{');  // Skip to the models object
        while (std::getline(file, line, '"')) {
            int id;
            file >> id;
            std::getline(file, line, ':'); // Skip to the model data
            model m;
            file >> m;
            model_datatable_map[id] = m;
            std::getline(file, line, ','); // Skip to the next model or end
            if (line.find("}") != std::string::npos) {
                break;
            }
        }
        file.close();
    }
}
