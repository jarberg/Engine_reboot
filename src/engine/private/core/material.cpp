#include <core/material.h>

#include "engine/core/GL.h"
#include <core/files/gltf.h>

void Material::Material::setTexture(std::string path, int index)
{
	tex = gltfImp::loadTexture2DFromPath(path.c_str(), /*flipY*/ true);
}

void Material::Material::bind() {
	shader->use();

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, tex);

}

