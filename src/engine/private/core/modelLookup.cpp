#include <core/modelLookup.h>
#include <fstream>
#include <iostream>

using json = nlohmann::json;


// JSON serialization
void to_json(json& j, const Model& m) {
    j = json{ {"name", m.name}, {"vertex", m.vertex_pos}, {"indices" , m.indices}, {"uvs", m.uvs }, {"normals", m.normals}};
}

void from_json(const json& j, Model& m) {
    j.at("name").get_to(m.name);
    j.at("vertex").get_to(m.vertex_pos);
    j.at("indices").get_to(m.indices);
    if (j.contains("uvs"))  j.at("uvs").get_to(m.uvs);
    if (j.contains("normals"))  j.at("normals").get_to(m.normals);
}


std::shared_ptr<Model> model_datatable::get_model(int id) {
    auto it = model_datatable_map.find(id);
    if (it != model_datatable_map.end()) {
        return it->second;
    }
    return error_model();
}

void model_datatable::add_model(std::shared_ptr<Model> new_model) {
    int new_id = generate_new_id();
    model_datatable_map.emplace(new_id, std::move(new_model));
    save_dataTable();
}

int model_datatable::generate_new_id() {
    if (model_datatable_map.empty()) {
        return 1;
    }
    return model_datatable_map.size() + 1;
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
        file << "\"" << id << "\": " << json(*model);
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
        auto m = std::make_shared<Model>(value.get<Model>()); // build from JSON
        model_datatable_map.emplace(id, std::move(m));
    }
}

std::shared_ptr<Model> model_datatable::error_model() {
    return std::make_shared<Model>(Model("error", { 1.0f, 0.0f, -1.0f }, { 0,1,2 }, { 1,0 }, {0.0,0.0,-1.0}));
}

