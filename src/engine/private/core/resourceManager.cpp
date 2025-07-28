#include "core/resourceManager.h"

ResourceManager::ResourceManager() {}

void ResourceManager::create_model(unsigned int _id, model m)
{
    // Create and setup glmodel
    glmodel glm = glmodel(_id, createBuffer(), createVertexArray(), m.vertex_pos.size(), m.vertex_pos);

    glm.bind();
    glm.upload();  // Upload data to GPU
    glm.unbind();  // Unbind VAO and VBO

    // Store the model in the resource manager
    this->buffers.emplace(_id, std::move(glm));
}

glmodel* ResourceManager::get_model(unsigned int _id)
{
    auto& m = this->buffers;
    auto ret_iterator = m.find(_id);

    if (ret_iterator != m.end()) {
        //std::cout << " : found : Value : " << ret_iterator->second.ID << std::endl;
        return &(ret_iterator->second);
    }
    return nullptr;
}

void glmodel::upload()
{
    glBindBuffer(GL_ARRAY_BUFFER, this->VBO);
    glBindVertexArray(this->VAO);

    // Upload vertex data
    glBufferData(GL_ARRAY_BUFFER, vertex_pos.size() * sizeof(float), vertex_pos.data(), GL_STATIC_DRAW);

    // Specify the layout of the vertex data
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);

    // Enable the vertex attribute at index 0 (position)
    glEnableVertexAttribArray(0);
}

void glmodel::bind()
{
    glBindVertexArray(this->VAO);
}

void glmodel::unbind()
{
    glBindVertexArray(0);
}
