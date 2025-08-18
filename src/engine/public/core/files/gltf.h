#pragma once
#include <core/types.h>
#include <engine/core/GL.h>

namespace gltfImp
{
	GLuint loadTexture2DFromPath(const char* path, bool flipY = true);
	void check(bool cond, const char* msg);
	void load_files(const std::string& path, std::shared_ptr<Model> &modelOut);
}


