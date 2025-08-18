#include "core/resourceManager.h"
#include <cstdint>
#include <iostream>

ResourceManager::ResourceManager() {}

void ResourceManager::create_model(unsigned int _id, const std::shared_ptr<Model> m) {
    // Create GPU objects for pos/EBO/VAO up front
    glmodel glm(_id, m);

    std::cout << "Uploading model: "
        << (glm.m->vertex_pos.size() / 3) << " verts, "
        << glm.indexCount << " indices, "
        << (glm.m->uvs.size() / 2) << " UV verts\n";

    glm.bind();
    glm.upload();   // Upload vertex / uv / index data
    glm.unbind();

    buffers.emplace(_id, std::move(glm));
}

glmodel* ResourceManager::get_model(unsigned int _id) {
    auto it = buffers.find(_id);
    return (it != buffers.end()) ? &it->second : nullptr;
}

glmodel::glmodel(unsigned int id, std::shared_ptr<Model> _m)
{
    VAO = ResourceManager::createVertexArray();
    VBO = ResourceManager::createBuffer();
    EBO = ResourceManager::createBuffer();
    NBO = ResourceManager::createBuffer();
    indexCount = _m->indices.size();

	ID = id;
    m = _m;
}

void glmodel::upload() {
    glBindVertexArray(VAO);

    // --- Positions (vec3), tightly packed in their own VBO ---
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER,
        static_cast<GLsizeiptr>(m->vertex_pos.size() * sizeof(float)),
        m->vertex_pos.data(),
        GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);                    // layout(location=0) in vec3 aPos;
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);

    // --- UVs (vec2) in a separate VBO (if present) ---
    if (!m->uvs.empty()) {
        if (VBOuv == 0) glGenBuffers(1, &VBOuv);     // ensure the buffer exists
        glBindBuffer(GL_ARRAY_BUFFER, VBOuv);
        glBufferData(GL_ARRAY_BUFFER,
            static_cast<GLsizeiptr>(m->uvs.size() * sizeof(float)),
            m->uvs.data(),
            GL_STATIC_DRAW);
        glEnableVertexAttribArray(1);                // layout(location=1) in vec2 aUV;
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, (void*)0);
    }

    if (!m->normals.empty()) {

        if (NBO == 0) glGenBuffers(1, &NBO);     // ensure the buffer exists
        glBindBuffer(GL_ARRAY_BUFFER, NBO);
        glBufferData(GL_ARRAY_BUFFER,
            static_cast<GLsizeiptr>(m->normals.size() * sizeof(float)),
            m->normals.data(),
            GL_STATIC_DRAW);
        glEnableVertexAttribArray(2);                // layout(location=1) in vec2 aUV;
        glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
    }

    // --- Indices (u16) ---
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER,
        static_cast<GLsizeiptr>(m->indices.size() * sizeof(uint16_t)),
        m->indices.data(),
        GL_STATIC_DRAW);

    // Update draw count (fall back to non-indexed if needed)
    indexCount = !m->indices.empty()
        ? m->indices.size()
        : m->vertex_pos.size() / 3; // number of vertices

#ifndef NDEBUG
    // Quick sanity: matching vertex counts?
    const size_t posVerts = m->vertex_pos.size() / 3;
    const size_t uvVerts = m->uvs.size() / 2;
    if (!m->uvs.empty() && posVerts != uvVerts) {
        std::cerr << "[glmodel::upload] WARNING: pos verts (" << posVerts
            << ") != uv verts (" << uvVerts << ")\n";
    }
#endif

    glBindVertexArray(0);
}

void glmodel::bind() { glBindVertexArray(VAO);

}
void glmodel::unbind() { glBindVertexArray(0); }
