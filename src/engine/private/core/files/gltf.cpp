// gltf.cpp — single implementation TU for tinygltf + stb_image
#define STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_WRITE_IMPLEMENTATION
#define TINYGLTF_IMPLEMENTATION

#include <core/files/gltf.h>
#include "tiny_gltf.h"
#include <engine/core/GL.h>

#include <stdexcept>
#include <cstdint>
#include <iostream>
#include <vector>
#include <string>

GLuint gltfImp::loadTexture2DFromPath(const char* path, bool flipY)
{
    int w, h, comp;
    stbi_set_flip_vertically_on_load(flipY ? 1 : 0);
    unsigned char* pixels = stbi_load(path, &w, &h, &comp, 4); // force RGBA
    gltfImp::check(pixels != nullptr, "Failed to load image (stb_image)");

    GLuint tex = 0;
    glGenTextures(1, &tex);
    glBindTexture(GL_TEXTURE_2D, tex);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, w, h, 0, GL_RGBA, GL_UNSIGNED_BYTE, pixels);

    // Optional: glGenerateMipmap(GL_TEXTURE_2D);

    stbi_image_free(pixels);
    return tex;
}

void gltfImp::check(bool cond, const char* msg)
{
    if (!cond) throw std::runtime_error(msg);
}


void test(tinygltf::Model model,std::string _attributename, std::vector<float> &out) {

    const auto& primitive = model.meshes[0].primitives[0];
    if (model.meshes.empty() || model.meshes[0].primitives.empty()) {
        std::cerr << "No meshes/primitives in glTF.\n";
        return;
    }

    auto posIt = primitive.attributes.find(_attributename);
    if (posIt == primitive.attributes.end()) {
        std::cerr << "Missing POSITION attribute.\n";
        return;
    }
    const auto& Accessor = model.accessors[posIt->second];
    const auto& BufferView = model.bufferViews[Accessor.bufferView];
    const auto& Buffer = model.buffers[BufferView.buffer];

    size_t posStride = Accessor.ByteStride(BufferView);
    const size_t posCompSize = sizeof(float);
    if (posStride == 0) posStride = 3 * posCompSize;

    const unsigned char* posBase =
        Buffer.data.data() + BufferView.byteOffset + Accessor.byteOffset;

    size_t compSize = 0;
    switch (Accessor.componentType) {
    case TINYGLTF_COMPONENT_TYPE_FLOAT: compSize = 4; break;
    case TINYGLTF_COMPONENT_TYPE_SHORT: compSize = 2; break; // signed
    case TINYGLTF_COMPONENT_TYPE_BYTE:  compSize = 1; break; // signed
    default:
        std::cerr << "Unsupported componentType: "
            << Accessor.componentType << "\n";
        compSize = 0;
    }
    const unsigned char* nBase =
        Buffer.data.data() + BufferView.byteOffset + Accessor.byteOffset;

    if (compSize) {
        size_t stride = Accessor.ByteStride(BufferView);
        if (stride == 0) stride = 3 * compSize; // VEC3

        const bool norm = Accessor.normalized; // glTF sets this for packed normals
        out.reserve(Accessor.count * 3);

        for (size_t i = 0; i < Accessor.count; ++i) {
            const unsigned char* p = nBase + i * stride;
            float x = 0.f, y = 0.f, z = 0.f;

            switch (Accessor.componentType) {
            case TINYGLTF_COMPONENT_TYPE_FLOAT: {
                const float* f = reinterpret_cast<const float*>(p);
                x = f[0]; y = f[1]; z = f[2];
            } break;
            case TINYGLTF_COMPONENT_TYPE_SHORT: {
                const int16_t* s = reinterpret_cast<const int16_t*>(p);
                if (norm) { x = s[0] / 32767.0f; y = s[1] / 32767.0f; z = s[2] / 32767.0f; }
                else { x = float(s[0]);     y = float(s[1]);     z = float(s[2]); }
            } break;
            case TINYGLTF_COMPONENT_TYPE_BYTE: {
                const int8_t* b = reinterpret_cast<const int8_t*>(p);
                if (norm) { x = b[0] / 127.0f; y = b[1] / 127.0f; z = b[2] / 127.0f; }
                else { x = float(b[0]);  y = float(b[1]);  z = float(b[2]); }
            } break;
            }

            // Safety renormalize
            float L = sqrtf(x * x + y * y + z * z);
            if (L > 0.0f) { x /= L; y /= L; z /= L; }

            out.push_back(x); out.push_back(y); out.push_back(z);
        }
    }
}


void gltfImp::load_files(const std::string& path, std::shared_ptr<Model> &modelOut) 
{
    tinygltf::TinyGLTF loader;
    tinygltf::Model model;
    std::string err, warn;

    bool result = false;
    if (path.size() >= 4 && path.substr(path.size() - 4) == ".glb") {
        result = loader.LoadBinaryFromFile(&model, &err, &warn, path);
    } else {
        result = loader.LoadASCIIFromFile(&model, &err, &warn, path);
    }

    if (!warn.empty()) std::cout << "warning: " << warn << std::endl;
    if (!err.empty())  std::cout << "error: "   << err  << std::endl;
    if (!result) {
        std::cout << "Failed to load glTF model from " << path << std::endl;
        return;
    }

    if (model.meshes.empty() || model.meshes[0].primitives.empty()) {
        std::cerr << "No meshes/primitives in glTF.\n";
        return;
    }

    std::cout << "Successfully loaded glTF model from " << path << std::endl;

    const auto& primitive = model.meshes[0].primitives[0];

    std::vector<float> positions;
    test(model, "POSITION", positions);

    std::vector<float> normals; // 3 floats per vertex
    test(model, "NORMAL", normals);

    std::vector<uint16_t> indices;

    std::vector<float> uvs;
    if (auto itUV = primitive.attributes.find("TEXCOORD_0"); itUV != primitive.attributes.end()) {
        const auto& uvAccessor   = model.accessors[itUV->second];
        const auto& uvBufferView = model.bufferViews[uvAccessor.bufferView];
        const auto& uvBuffer     = model.buffers[uvBufferView.buffer];

        const unsigned char* uvBase =
            uvBuffer.data.data() + uvBufferView.byteOffset + uvAccessor.byteOffset;

        size_t compSize = 0;
        switch (uvAccessor.componentType) {
            case TINYGLTF_COMPONENT_TYPE_FLOAT:          compSize = 4; break;
            case TINYGLTF_COMPONENT_TYPE_UNSIGNED_SHORT: compSize = 2; break;
            case TINYGLTF_COMPONENT_TYPE_UNSIGNED_BYTE:  compSize = 1; break;
            default:
                std::cerr << "Unsupported UV componentType: " << uvAccessor.componentType << "\n";
                compSize = 0;
                break;
        }
        if (compSize != 0) {
            size_t uvStride = uvAccessor.ByteStride(uvBufferView);
            if (uvStride == 0) uvStride = 2 * compSize; // VEC2

            const bool norm = uvAccessor.normalized;
            uvs.reserve(uvAccessor.count * 2);

            for (size_t i = 0; i < uvAccessor.count; ++i) {
                const unsigned char* p = uvBase + i * uvStride;
                float u = 0.f, v = 0.f;

                switch (uvAccessor.componentType) {
                    case TINYGLTF_COMPONENT_TYPE_FLOAT: {
                        const float* f = reinterpret_cast<const float*>(p);
                        u = f[0]; v = f[1];
                    } break;
                    case TINYGLTF_COMPONENT_TYPE_UNSIGNED_SHORT: {
                        const uint16_t* s = reinterpret_cast<const uint16_t*>(p);
                        if (norm) { u = s[0] / 65535.0f; v = s[1] / 65535.0f; }
                        else      { u = static_cast<float>(s[0]); v = static_cast<float>(s[1]); }
                    } break;
                    case TINYGLTF_COMPONENT_TYPE_UNSIGNED_BYTE: {
                        const uint8_t* b = reinterpret_cast<const uint8_t*>(p);
                        if (norm) { u = b[0] / 255.0f; v = b[1] / 255.0f; }
                        else      { u = static_cast<float>(b[0]); v = static_cast<float>(b[1]); }
                    } break;
                }

                // If your textures appear upside-down, consider:
                // v = 1.0f - v;

                uvs.push_back(u);
                uvs.push_back(v);
            }
        }
    }
    
    if (primitive.indices >= 0) {
        const auto& idxAccessor   = model.accessors[primitive.indices];
        const auto& idxBufferView = model.bufferViews[idxAccessor.bufferView];
        const auto& idxBuffer     = model.buffers[idxBufferView.buffer];

        const unsigned char* idxBase =
            idxBuffer.data.data() + idxBufferView.byteOffset + idxAccessor.byteOffset;

        size_t compSize = 0;
        switch (idxAccessor.componentType) {
            case TINYGLTF_COMPONENT_TYPE_UNSIGNED_SHORT: compSize = 2; break;
            case TINYGLTF_COMPONENT_TYPE_UNSIGNED_BYTE:  compSize = 1; break;
            case TINYGLTF_COMPONENT_TYPE_UNSIGNED_INT:   compSize = 4; break;
            default:
                std::cerr << "Unsupported index component type.\n";
                compSize = 0;
                break;
        }

        if (compSize != 0) {
            size_t idxStride = idxAccessor.ByteStride(idxBufferView);
            if (idxStride == 0) idxStride = compSize;

            indices.reserve(idxAccessor.count);
            for (size_t i = 0; i < idxAccessor.count; ++i) {
                const unsigned char* p = idxBase + i * idxStride;
                switch (idxAccessor.componentType) {
                    case TINYGLTF_COMPONENT_TYPE_UNSIGNED_SHORT:
                        indices.push_back(*reinterpret_cast<const uint16_t*>(p));
                        break;
                    case TINYGLTF_COMPONENT_TYPE_UNSIGNED_BYTE:
                        indices.push_back(static_cast<uint16_t>(*reinterpret_cast<const uint8_t*>(p)));
                        break;
                    case TINYGLTF_COMPONENT_TYPE_UNSIGNED_INT:
                        indices.push_back(static_cast<uint16_t>(*reinterpret_cast<const uint32_t*>(p)));
                        break;
                }
            }
        }
    }
    
    modelOut = std::make_shared<Model>(model.meshes[0].name, positions, indices, uvs, normals);
    modelOut->path = path;
}
