#pragma once
#include "engine/core/singleton.h"

#include "engine/core/GL.h"

#include "engine/core/types.h"

#include <vector>
#include <map>

struct glmodel {
    unsigned int ID;
    unsigned int VBO;
    unsigned int VAO;
    size_t vertCount;
    std::vector<float> vertex_pos;
    glmodel() {
        ID = 25;
        VBO = 25;
        VAO = 25;
        vertCount = 0;
        vertex_pos = {};
    };
    glmodel(unsigned int _ID, unsigned int _VBO, unsigned int _VAO, size_t _vertCount, const std::vector<float>& _vertex_pos = {})
        : ID(_ID), VBO(_VBO), VAO(_VAO), vertCount(_vertCount), vertex_pos(_vertex_pos) {}

    void upload();
    void bind();
    void unbind();

    // Define the equality operator
    bool operator==(const glmodel& other) const {
        // Implement your comparison logic here
        // For example, compare all relevant members:
        return ID == other.ID;/* comparison of members */;
    }
};

class ResourceManager:public Singleton<ResourceManager> {
    friend class Singleton<ResourceManager>;

private:
    std::map<unsigned int, glmodel> buffers;
    ResourceManager();


public:

    void create_model(unsigned int _id, model m); 
    
    glmodel* get_model(unsigned int _id);

    unsigned int createBuffer() {
        unsigned int buffer;
        glGenBuffers(1, &buffer);
        return buffer;
    }

    unsigned int createVertexArray() {
        unsigned int buffer;
        glGenVertexArrays(1, &buffer);
        return buffer;
    }

    void deleteBuffer(unsigned int buffer) {
        glDeleteBuffers(1, &buffer);
        buffers.erase(buffer);
    }

    ~ResourceManager() {
        for (auto model : buffers) {
            glDeleteBuffers(1, &model.second.VAO);
            glDeleteBuffers(1, &model.second.VBO);
        }
    }

};
