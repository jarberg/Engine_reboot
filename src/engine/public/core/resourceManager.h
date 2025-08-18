#pragma once

#include <vector>
#include <map>
#include <iostream>

#include "core/singleton.h"
#include "core/types.h"
#include "engine/core/GL.h"

struct glmodel {
    unsigned int ID = 0;
    unsigned int VAO = 0;
    unsigned int VBO = 0;   // position
    unsigned int NBO = 0;
    unsigned int EBO = 0;   // indices
    unsigned int VBOuv = 0;   // UVs (created in upload() if needed)

    std::shared_ptr<Model> m;

    size_t indexCount = 0;

    glmodel() = default;
    glmodel(unsigned int id, std::shared_ptr<Model> _m) ;

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
    void create_model(unsigned int _id, std::shared_ptr<Model> m);
    glmodel* get_model(unsigned int _id);

    // Raw GL helpers (not tied to the map)
    static unsigned int createBuffer() { unsigned int b = 0; glGenBuffers(1, &b); return b; }
    static unsigned int createVertexArray() { unsigned int a = 0; glGenVertexArrays(1, &a); return a; }

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
