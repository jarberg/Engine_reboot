#include <core/modelLookup.h>
#include <fstream>
#include <iostream>

using json = nlohmann::json;


// JSON serialization
void to_json(json& j, const Model& m) {
    j = json{ {"name", m.name}, {"vertex", m.vertex_pos}, {"indices" , m.indices}, {"uvs", m.uvs } };
}

void from_json(const json& j, Model& m) {
    j.at("name").get_to(m.name);
    j.at("vertex").get_to(m.vertex_pos);
    j.at("indices").get_to(m.indices);
    if (j.contains("uvs"))  j.at("uvs").get_to(m.uvs);
}


Model model_datatable::get_model(int id) {
    auto it = model_datatable_map.find(id);
    if (it != model_datatable_map.end()) {
        return it->second;
    }
    return error_model();
}

void model_datatable::add_model(Model new_model) {
    int new_id = generate_new_id();
    model_datatable_map.emplace(new_id, std::move(new_model));
    save_dataTable();
}

void model_datatable::add_model(std::string _name, std::vector<float> _points, std::vector<unsigned short> _indices, std::vector<float> _uvs ) {
    Model new_model(std::move(_name), std::move(_points), std::move(_indices), std::move(_uvs));
    add_model(new_model);
}

int model_datatable::generate_new_id() {
    if (model_datatable_map.empty()) {
        return 1;
    }
    return model_datatable_map.rbegin()->first + 1;
}

void model_datatable::save_dataTable() {
    std::ofstream file("resources/models.json");
    if (!file.is_open()) {
        std::cerr << "Failed to open models.json for writing\n";
        return;
    }

    file << "{ \"models\": {";
    bool first = true;
    for (const auto& [id, model] : model_datatable_map) {
        if (!first) file << ", ";
        file << "\"" << id << "\": " << json(model);
        first = false;
    }
    file << "} }";
    file.close();
}

void model_datatable::load_dataTable() {
    std::ifstream file("resources/models.json");
    if (!file.is_open()) {
        std::cerr << "Failed to open models.json\n";
        return;
    }

    json j;
    file >> j;

    if (!j.contains("models")) {
        std::cerr << "JSON does not contain 'models'\n";
        return;
    }

    for (auto& [key, value] : j["models"].items()) {
        int id = std::stoi(key);
        Model m = value.get<Model>();
        model_datatable_map[id] = std::move(m);
    }
}

Model model_datatable::error_model() {
    return Model("error", { 1.0f, 0.0f, -1.0f }, { 0,1,2 }, {1,0});
}

