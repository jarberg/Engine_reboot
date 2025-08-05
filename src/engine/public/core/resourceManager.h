#pragma once

#include <vector>
#include <map>
#include <iostream>

#include "core/singleton.h"
#include "core/types.h"
#include "engine/core/GL.h"

struct glmodel {
    unsigned int ID;
    unsigned int VBO;
    unsigned int VAO;
    unsigned int EBO;
    size_t indexCount;
    std::vector<float> vertex_pos;
    std::vector<uint16_t> indices;

    glmodel()
        : ID(0), VBO(0), VAO(0), EBO(0), indexCount(0) {
    }

    glmodel(unsigned int _ID, unsigned int _VBO, unsigned int _VAO, unsigned int _EBO,
        size_t _indexCount, const std::vector<float>& _vertex_pos = {}, const std::vector<uint16_t>& _indices = {})
        : ID(_ID), VBO(_VBO), VAO(_VAO), EBO(_EBO), indexCount(_indexCount), vertex_pos(_vertex_pos), indices(_indices) {
    }

    void upload();
    void bind();
    void unbind();

    bool operator==(const glmodel& other) const {
        return ID == other.ID;
    }
};

class ResourceManager : public Singleton<ResourceManager> {
    friend class Singleton<ResourceManager>;

private:
    std::map<unsigned int, glmodel> buffers;
    ResourceManager();

public:
    void create_model(unsigned int _id, Model m);
    glmodel* get_model(unsigned int _id);

    unsigned int createBuffer() {
        unsigned int buffer;
        glGenBuffers(1, &buffer);
        return buffer;
    }

    unsigned int createVertexArray() {
        unsigned int array;
        glGenVertexArrays(1, &array);
        return array;
    }

    void deleteBuffer(unsigned int buffer) {
        glDeleteBuffers(1, &buffer);
        buffers.erase(buffer);
    }

    ~ResourceManager() {
        for (auto& model : buffers) {
            glDeleteBuffers(1, &model.second.VBO);
            glDeleteBuffers(1, &model.second.EBO);
            glDeleteVertexArrays(1, &model.second.VAO);
        }
    }
};
