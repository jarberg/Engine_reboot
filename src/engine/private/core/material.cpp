#include <core/material.h>

#include "engine/core/GL.h"

void Material::Material::bind() {
	shader->use();
}

