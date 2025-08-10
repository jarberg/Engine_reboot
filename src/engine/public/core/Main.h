#pragma once

#include "entt.hpp"

#include <core/input.h>

#include "core/entity.h"
#include "core/Events.h"
#include "core/world.h"
#include "core/fileLoader.h"
#include <core/extensions.h>

#include "core/singleton.h"
#include "core/modelLookup.h"
#include <core/resourceManager.h>
#include <core/types.h>


#include <iostream>
#include <chrono>

#define STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_WRITE_IMPLEMENTATION
#define TINYGLTF_IMPLEMENTATION

#include "tiny_gltf.h"

#include "engine/core/GL.h"


extern World* myWorld;
std::array<float, 16> projection;

float width = 1280;
float height = 720;

ResourceManager* RMan;
unsigned int shaderProgram;
Entity PlayerEntity;

int main();

void render(entt::registry& registry) {
	auto view = registry.view<StaticMeshComponent, PositionComponent, RotationComponent>();

	glClearColor(0.5, 0.5, 0.5, 1);
	glClear(GL_COLOR_BUFFER_BIT);

	glUseProgram(shaderProgram);

	glEnable(GL_CULL_FACE);
	glFrontFace(GL_CCW);
	glCullFace(GL_BACK);

	auto cameraview = registry.view<CameraComponent>();
	CameraComponent& cam = cameraview.get<CameraComponent>(cameraview.front());

	for (auto entity : view) {
		auto& buffer = view.get<StaticMeshComponent>(entity);
		glmodel* m = RMan->get_model(buffer.meshID);
		if (m) {
			m->bind();

			GLint perspLoc = glGetUniformLocation(shaderProgram, "uPersp");
			glUniformMatrix4fv(perspLoc, 1, GL_FALSE, cam.perspectiveMatrix.data());
			GLint cameraLoc = glGetUniformLocation(shaderProgram, "uCamera");
			glUniformMatrix4fv(cameraLoc, 1, GL_FALSE, cam.cameraMatrix.data());

			auto& position = view.get<PositionComponent>(entity);
			auto& rotation = view.get<RotationComponent>(entity);

			rotation.matrix[12] = static_cast<float>(position.x);
			rotation.matrix[13] = static_cast<float>(position.y);
			rotation.matrix[14] = static_cast<float>(position.z);
			rotation.matrix[15] = 1.0f;

			GLint transformLoc = glGetUniformLocation(shaderProgram, "uTransform");
			glUniformMatrix4fv(transformLoc, 1, GL_FALSE, rotation.matrix);

			glDrawElements(GL_TRIANGLES, m->indexCount, GL_UNSIGNED_SHORT, 0);

			m->unbind();
		}
	}
}



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


void init() {

	std::string vsPath = "resources/shaders/test" + std::string(vertexShader_ext);
	std::string fsPath = "resources/shaders/test" + std::string(fragmentShader_ext);

	const char* vertexShaderSource = read_file<char*>(vsPath);
	const char* fragmentShaderSource = read_file<char*>(fsPath);
	

	unsigned int vertexShader = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
	glCompileShader(vertexShader);

	int success;
	char infoLog[512];
	glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
	if (!success) {
		glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
		std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;
	}

	// Create and compile the fragment shader
	unsigned int fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
	glCompileShader(fragmentShader);
	glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
	if (!success) {
		glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
		std::cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" << infoLog << std::endl;
	}

	// Link shaders into a program
	shaderProgram = glCreateProgram();
	glAttachShader(shaderProgram, vertexShader);
	glAttachShader(shaderProgram, fragmentShader);
	glLinkProgram(shaderProgram);
	glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
	if (!success) {
		glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
		std::cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << std::endl;
	}

	glUseProgram(shaderProgram);
	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader);


	auto resourceManager = Singleton<ResourceManager>::GetInstance();

	auto* dataTable = model_datatable::GetInstance();

	RMan = ResourceManager::GetInstance();

	dataTable->load_dataTable();

	for (auto& [key, value] : dataTable->get_model_map()) {
		RMan->create_model(key, value);
	}
	/*
	Model mod;
	load_files("resources/models/Untitled.gltf", mod);
	RMan->create_model(3, mod);
	dataTable->add_model(mod);
	*/
	dataTable->print_data();

	PlayerEntity = myWorld->create_entity("test");
	myWorld->add_component<StaticMeshComponent>(PlayerEntity,2);
	myWorld->add_component<CharacterComponent>(PlayerEntity, PlayerEntity);
	myWorld->add_component<RotationComponent>(PlayerEntity);

	Entity cameraEntity = myWorld->create_entity("camera");
	myWorld->add_component<CameraComponent>(cameraEntity, 90.0f, width/height, 0.1f, 100.0f);

	std::cout << "Hello, world!" << std::endl;
}



