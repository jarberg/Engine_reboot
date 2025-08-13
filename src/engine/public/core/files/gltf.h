#pragma once


#define STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_WRITE_IMPLEMENTATION
#define TINYGLTF_IMPLEMENTATION

#include "tiny_gltf.h"
#include <core/types.h>

void load_files(const std::string& path, Model& modelOut) {
	tinygltf::TinyGLTF loader;
	tinygltf::Model model;
	std::string err, warn;

	bool result = false;
	if (path.substr(path.size() - 4) == ".glb") {
		result = loader.LoadBinaryFromFile(&model, &err, &warn, path);
	}
	else {
		result = loader.LoadASCIIFromFile(&model, &err, &warn, path);
	}

	if (!warn.empty()) std::cout << "warning: " << warn << std::endl;
	if (!err.empty()) std::cout << "error: " << err << std::endl;
	if (!result) {
		std::cout << "Failed to load glTF model from " << path << std::endl;
		return;
	}

	std::cout << "Successfully loaded glTF model from " << path << std::endl;

	const auto& primitive = model.meshes[0].primitives[0];

	// ========== POSITIONS ==========
	const auto& posAccessor = model.accessors[primitive.attributes.at("POSITION")];
	const auto& posBufferView = model.bufferViews[posAccessor.bufferView];
	const auto& posBuffer = model.buffers[posBufferView.buffer];

	size_t posStride = posAccessor.ByteStride(posBufferView);
	if (posStride == 0) posStride = sizeof(float) * 3;

	const unsigned char* posData = posBuffer.data.data() + posBufferView.byteOffset + posAccessor.byteOffset;

	std::vector<float> positions;
	for (size_t i = 0; i < posAccessor.count; ++i) {
		const float* v = reinterpret_cast<const float*>(posData + i * posStride);
		positions.push_back(v[0]);
		positions.push_back(v[1]);
		positions.push_back(v[2]);
	}

	// ========== INDICES ==========
	std::vector<uint16_t> indices;
	if (primitive.indices > -1) {
		const auto& indexAccessor = model.accessors[primitive.indices];
		const auto& indexBufferView = model.bufferViews[indexAccessor.bufferView];
		const auto& indexBuffer = model.buffers[indexBufferView.buffer];

		const unsigned char* indexData = indexBuffer.data.data() + indexBufferView.byteOffset + indexAccessor.byteOffset;

		for (size_t i = 0; i < indexAccessor.count; ++i) {
			switch (indexAccessor.componentType) {
			case TINYGLTF_COMPONENT_TYPE_UNSIGNED_SHORT:
				indices.push_back(reinterpret_cast<const uint16_t*>(indexData)[i]);
				break;
			case TINYGLTF_COMPONENT_TYPE_UNSIGNED_BYTE:
				indices.push_back(static_cast<uint16_t>(reinterpret_cast<const uint8_t*>(indexData)[i]));
				break;
			case TINYGLTF_COMPONENT_TYPE_UNSIGNED_INT:
				indices.push_back(static_cast<uint16_t>(reinterpret_cast<const uint32_t*>(indexData)[i]));
				break;
			default:
				std::cerr << "Unsupported index component type.\n";
				return;
			}
		}
	}

	modelOut = Model(model.meshes[0].name, positions, indices);
}

