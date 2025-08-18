#pragma once
#include <map>
#include <string>
#include <vector>
#include <iostream>
#include <fstream>
#include <filesystem>


struct Model {
    std::string name;
    std::string path;
    std::vector<unsigned short> indices;
    std::vector<float> vertex_pos;
    std::vector<float> uvs;
    std::vector<float> normals;
    size_t vertexCount = 0;


    Model() = default;
    Model(const std::string& name, const std::vector<float>& vertex_pos, std::vector<unsigned short> indices, const std::vector<float>& _uvs, const std::vector<float>& _normals)
        : name(name), vertex_pos(vertex_pos), indices(indices), uvs(_uvs), normals(_normals) {
        vertexCount = vertex_pos.size();
    }

    Model(const Model& other) = default;
    Model(Model&& other) noexcept = default;

    Model& operator=(const Model& other) = default;
    Model& operator=(Model&& other) noexcept = default;

    ~Model() = default;

    void addUvs(std::vector<float> _uvs) {
        uvs = _uvs;
    }

    friend std::ostream& operator<<(std::ostream& os, const Model& m) {
        os << "{ \"name\": \"" << m.name << "\", \"vertex\": [";
        for (size_t i = 0; i < m.vertex_pos.size(); ++i) {
            os << m.vertex_pos[i];
            if (i != m.vertex_pos.size() - 1) {
                os << ", ";
            }
        }
        os << "] }";
        return os;
    }



    friend std::istream& operator>>(std::istream& is, Model& m) {
        // Basic deserialization logic (you can improve it)
        std::string line;
        std::getline(is, line, '"');  // Skip the opening brace and name key
        std::getline(is, m.name, '"'); // Read the name value
        std::getline(is, line, '[');  // Skip to vertex array
        m.vertex_pos.clear();
        float vertex;
        while (is >> vertex) {
            m.vertex_pos.push_back(vertex);
            if (is.peek() == ',') {
                is.ignore(); // Skip the comma
            }
            if (is.peek() == ']') {
                is.ignore(); // Skip the closing bracket
                break;
            }
        }
        std::getline(is, line, '}'); // Skip the closing brace

        return is;
    }
};
