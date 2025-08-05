#include "core/resourceManager.h"

ResourceManager::ResourceManager() {}

void ResourceManager::create_model(unsigned int _id, Model m) {
    glmodel glm = glmodel(
        _id,
        createBuffer(),        // VBO
        createVertexArray(),   // VAO
        createBuffer(),        // EBO
        m.indices.size(),
        m.vertex_pos,
        m.indices
    );

    std::cout << "Uploading model with " << glm.indexCount << " indices.\n";

    glm.bind();
    glm.upload();  // Upload vertex and index data to GPU
    glm.unbind();

    buffers.emplace(_id, std::move(glm));
}

glmodel* ResourceManager::get_model(unsigned int _id) {
    auto it = buffers.find(_id);
    if (it != buffers.end()) {
        return &it->second;
    }
    return nullptr;
}

void glmodel::upload() {
    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, vertex_pos.size() * sizeof(float), vertex_pos.data(), GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned short), indices.data(), GL_STATIC_DRAW);
}

void glmodel::bind() {
    glBindVertexArray(VAO);
}

void glmodel::unbind() {
    glBindVertexArray(0);
}
