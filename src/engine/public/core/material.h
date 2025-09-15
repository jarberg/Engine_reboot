#pragma once
#include <string>
#include "core/shader.h"


// material.h
namespace Material {
    struct Material {
        std::string name;
        Shader* shader;          // replace unsigned int shaPro
        GLuint tex;
        // common material params, e.g. baseColor, roughness, textures…
        Material(Shader* shader) : shader(shader) {};

        void setTexture(std::string path, int index);

        void bind();// and set material uniforms here
    };

    static Material* defaultMat;
}
