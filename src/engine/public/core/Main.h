#pragma once

#include "entt.hpp"

#include <core/input.h>

#include "core/entity.h"
#include "core/world.h"
#include "core/fileLoader.h"
#include <core/extensions.h>

#include "core/singleton.h"
#include "core/modelLookup.h"
#include <core/resourceManager.h>
#include <core/types.h>


#include <iostream>
#include <chrono>


#include "engine/core/GL.h"


extern World* myWorld;


ResourceManager* RMan;
unsigned int shaderProgram;
Entity PlayerEntity;

int main();

void render(entt::registry& registry) {
	auto view = registry.view<StaticMeshComponent, PositionComponent, RotationComponent>();
	glUseProgram(shaderProgram);
	for (auto entity : view) {
		auto& buffer = view.get<StaticMeshComponent>(entity);
		auto& position = view.get<PositionComponent>(entity);
		auto& rotation = view.get<RotationComponent>(entity);

		glmodel* m = RMan->get_model(buffer.meshID);

		if (m) {
			m->bind();
			rotation.matrix[12] = static_cast<float>(position.x);
			rotation.matrix[13] = static_cast<float>(position.y);
			rotation.matrix[14] = static_cast<float>(position.z);
			rotation.matrix[15] = 1.0f;

			GLint transformLoc = glGetUniformLocation(shaderProgram, "uTransform");

			glUniformMatrix4fv(transformLoc, 1, GL_FALSE, rotation.matrix);

			glDrawArrays(GL_TRIANGLE_STRIP, 0, m->vertCount);
			m->unbind();
		}
	}
}

void init() {

	myWorld = new World();

	std::string vsPath = "resources/shaders/test" + std::string(vertexShader_ext);
	std::string fsPath = "resources/shaders/test" + std::string(fragmentShader_ext);
	std::cout << "Vertex Shader path:\n" << vsPath << std::endl;
	std::cout << "fragment Shader Source:\n" << fsPath << std::endl;
	// Create and compile the vertex shader
	const char* vertexShaderSource = read_file<char*>(vsPath);
	const char* fragmentShaderSource = read_file<char*>(fsPath);
	std::cout << "Vertex Shader Source:\n" << vertexShaderSource << std::endl;
	std::cout << "Fragment Shader Source:\n" << fragmentShaderSource << std::endl;
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

	PlayerEntity = myWorld->create_entity("test");
	myWorld->add_component<StaticMeshComponent>(PlayerEntity,1);
	myWorld->add_component<CharacterComponent>(PlayerEntity, PlayerEntity);
	myWorld->add_component<RotationComponent>(PlayerEntity);



	std::cout << "Hello, world!" << std::endl;
}



