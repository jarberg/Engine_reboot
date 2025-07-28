#pragma once

#include "engine/public/core/entity.h"
#include "engine/public/core/world.h"
#include "engine/public/core/fileLoader.h"
#include <engine/public/core/resourceManager.h>
#include "engine/public/core/singleton.h"
#include "engine/public/core/modelLookup.h"
#include <engine/public/core/types.h>
#include <engine/public/core/extensions.h>

#include <iostream>
#include <chrono>
#include <string>

#include "engine/core/GL.h"
#include "entt.hpp"


World myWorld;
ResourceManager* RMan;
unsigned int shaderProgram;
auto lastTime = std::chrono::high_resolution_clock::now();
auto currentTime = std::chrono::high_resolution_clock::now();
std::chrono::duration<float> deltaTime;



inline void render(entt::registry& registry) {
	auto view = registry.view<engine::StaticMeshComponent>();
	glUseProgram(shaderProgram);
	for (auto entity : view) {
		auto& buffer = view.get<engine::StaticMeshComponent>(entity);

		glmodel* m = RMan->get_model(buffer.meshID);

		if (m) {
			m->bind();
			glDrawArrays(GL_TRIANGLE_STRIP, 0, m->vertCount);
			m->unbind();
		}

	}
}

inline void init() {

	std::string vsPath = "Resources/Shader/test" + std::string(vertexShader_ext);
	// Create and compile the vertex shader
	const char* vertexShaderSource = read_file<char*>(vsPath);

	const char* fragmentShaderSource = read_file<char*>("Resources/Shader/test" + std::string(fragmentShader_ext));


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

	auto* dataTable = model_datatable::GetInstance();

	RMan = ResourceManager::GetInstance();


	dataTable->load_dataTable();
	for (auto& [key, value] : dataTable->get_model_map()) {
		RMan->create_model(key, value);
	}

	Entity myEntity = myWorld.create_entity("test");

	//myEntity.add_component<VelocityComponent>();
	//myEntity.add_component<PositionComponent>(0.0f, 10.0f, 0.0f);
	//myEntity.add_component<AccelerationComponent>();
	myEntity.add_component<engine::StaticMeshComponent>(1);
	std::cout << "Hello, world!" << std::endl;
}

inline int mainMethod() {
	const int width = 800;
	const int height = 600;

	GLFWwindow* window = initWindow(width, height, "GLFW Window Example");
	if (!window) return -1;

	init();
	mainLoop(window);

	// Clean up and exit
	glfwDestroyWindow(window);
	glfwTerminate();
	return 0;
}

inline int mainLoop(GLFWwindow* window) {
	while (!glfwWindowShouldClose(window)) {
		currentTime = std::chrono::high_resolution_clock::now();

		deltaTime = currentTime - lastTime;
		lastTime = currentTime;

		// Clear the screen to black
		glClearColor(0.5f, 0.5f, 0.5f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);


		render(myWorld.get_registry());

		// Swap buffers and poll IO events
		glfwSwapBuffers(window);
		glfwPollEvents();
		myWorld.update(deltaTime.count());
	}
}

// Function to initialize GLFW and create a window
inline GLFWwindow* initWindow(const int width, const int height, const char* title) {
	if (!glfwInit()) {
		std::cerr << "Failed to initialize GLFW" << std::endl;
		return nullptr;
	}

	// Set GLFW window creation hints (optional)
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_VISIBLE, GLFW_TRUE); // Ensure the window is visible

	// Create a windowed mode window and its OpenGL context
	GLFWwindow* window = glfwCreateWindow(width, height, title, NULL, NULL);
	if (!window) {
		std::cerr << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return nullptr;
	}

	// Make the window's context current
	glfwMakeContextCurrent(window);

	// Load OpenGL functions using GLAD
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
		std::cerr << "Failed to initialize GLAD" << std::endl;
		glfwDestroyWindow(window);
		glfwTerminate();
		return nullptr;
	}

	// Enable VSync
	glfwSwapInterval(1);

	// Optional: GLFW callbacks for key handling, mouse movement, etc.
	glfwSetKeyCallback(window, [](GLFWwindow* window, int key, int scancode, int action, int mods) {
		if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
			glfwSetWindowShouldClose(window, GLFW_TRUE);
		});

	return window;
}