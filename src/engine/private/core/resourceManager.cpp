#include "core/resourceManager.h"
#include <cstdint>
#include <iostream>

ResourceManager::ResourceManager() {}

void ResourceManager::create_model(unsigned int _id, Model m) {
    // Create GPU objects for pos/EBO/VAO up front
    glmodel glm = glmodel(
        _id,
        createBuffer(),        // VBO (positions)
        createVertexArray(),   // VAO
        createBuffer(),        // EBO (indices)
        m.indices.size(),
        m.vertex_pos,
        m.indices,
        m.uvs                  // <-- use the correct field name
    );

    std::cout << "Uploading model: "
        << (glm.vertex_pos.size() / 3) << " verts, "
        << glm.indexCount << " indices, "
        << (glm.UvsList.size() / 2) << " UV verts\n";

    glm.bind();
    glm.upload();   // Upload vertex / uv / index data
    glm.unbind();

    buffers.emplace(_id, std::move(glm));
}

glmodel* ResourceManager::get_model(unsigned int _id) {
    auto it = buffers.find(_id);
    return (it != buffers.end()) ? &it->second : nullptr;
}

void glmodel::upload() {
    glBindVertexArray(VAO);

    // --- Positions (vec3), tightly packed in their own VBO ---
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER,
        static_cast<GLsizeiptr>(vertex_pos.size() * sizeof(float)),
        vertex_pos.data(),
        GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);                    // layout(location=0) in vec3 aPos;
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);

    // --- UVs (vec2) in a separate VBO (if present) ---
    if (!UvsList.empty()) {
        if (VBOuv == 0) glGenBuffers(1, &VBOuv);     // ensure the buffer exists
        glBindBuffer(GL_ARRAY_BUFFER, VBOuv);
        glBufferData(GL_ARRAY_BUFFER,
            static_cast<GLsizeiptr>(UvsList.size() * sizeof(float)),
            UvsList.data(),
            GL_STATIC_DRAW);
        glEnableVertexAttribArray(1);                // layout(location=1) in vec2 aUV;
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, (void*)0);
    }

    // --- Indices (u16) ---
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER,
        static_cast<GLsizeiptr>(indices.size() * sizeof(uint16_t)),
        indices.data(),
        GL_STATIC_DRAW);

    // Update draw count (fall back to non-indexed if needed)
    indexCount = !indices.empty()
        ? indices.size()
        : vertex_pos.size() / 3; // number of vertices

#ifndef NDEBUG
    // Quick sanity: matching vertex counts?
    const size_t posVerts = vertex_pos.size() / 3;
    const size_t uvVerts = UvsList.size() / 2;
    if (!UvsList.empty() && posVerts != uvVerts) {
        std::cerr << "[glmodel::upload] WARNING: pos verts (" << posVerts
            << ") != uv verts (" << uvVerts << ")\n";
    }
#endif

    glBindVertexArray(0);
}

void glmodel::bind() { glBindVertexArray(VAO);

}
void glmodel::unbind() { glBindVertexArray(0); }
