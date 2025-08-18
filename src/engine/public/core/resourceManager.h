#pragma once

#include <vector>
#include <map>
#include <iostream>

#include "core/singleton.h"
#include "core/types.h"
#include "engine/core/GL.h"

struct glmodel {
    unsigned int ID = 0;
    unsigned int VBO = 0;   // positions
    unsigned int VAO = 0;
    unsigned int EBO = 0;   // indices
    unsigned int VBOuv = 0;   // UVs (created in upload() if needed)

    size_t indexCount = 0;

    std::vector<float>    vertex_pos;   // 3 floats per vertex
    std::vector<float>    UvsList;      // 2 floats per vertex
    std::vector<uint16_t> indices;

    glmodel() = default;

    // No UVBO param here — it will be generated on demand in upload()
    glmodel(unsigned int _ID, unsigned int _VBO, unsigned int _VAO, unsigned int _EBO,
        size_t _indexCount,
        const std::vector<float>& _vertex_pos = {},
        const std::vector<uint16_t>& _indices = {},
        const std::vector<float>& _uvs = {})
        : ID(_ID), VBO(_VBO), VAO(_VAO), EBO(_EBO),
        indexCount(_indexCount), vertex_pos(_vertex_pos), UvsList(_uvs), indices(_indices) {
    }

    void upload();
    void bind();
    void unbind();
};

class ResourceManager : public Singleton<ResourceManager> {
    friend class Singleton<ResourceManager>;
private:
    // modelID -> glmodel
    std::map<unsigned int, glmodel> buffers;
    ResourceManager();

public:
    void create_model(unsigned int _id, Model m);
    glmodel* get_model(unsigned int _id);

    // Raw GL helpers (not tied to the map)
    unsigned int createBuffer() { unsigned int b = 0; glGenBuffers(1, &b); return b; }
    unsigned int createVertexArray() { unsigned int a = 0; glGenVertexArrays(1, &a); return a; }

    // Explicit model removal by model ID (frees VAO/VBOs/EBO)
    void remove_model(unsigned int modelId);

    ~ResourceManager() {
        for (auto& kv : buffers) {
            auto& mdl = kv.second;
            if (mdl.VBO)   glDeleteBuffers(1, &mdl.VBO);
            if (mdl.VBOuv) glDeleteBuffers(1, &mdl.VBOuv);
            if (mdl.EBO)   glDeleteBuffers(1, &mdl.EBO);
            if (mdl.VAO)   glDeleteVertexArrays(1, &mdl.VAO);
        }
    }
};
