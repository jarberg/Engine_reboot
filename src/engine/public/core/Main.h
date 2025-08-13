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
#include <core/files/gltf.h>

#include <iostream>
#include <chrono>



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
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);
	glEnable(GL_CULL_FACE);
	//glFrontFace(GL_CCW);
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
	myWorld->add_component<StaticMeshComponent>(PlayerEntity,3);
	myWorld->add_component<CharacterComponent>(PlayerEntity, PlayerEntity);
	myWorld->add_component<RotationComponent>(PlayerEntity);

	Entity cameraEntity = myWorld->create_entity("camera");
	myWorld->add_component<CameraComponent>(cameraEntity, 90.0f, width/height, 0.1f, 100.0f);

	std::cout << "Hello, world!" << std::endl;
}



