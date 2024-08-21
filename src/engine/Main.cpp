#include "engine/Main.h"
#include "engine/core/singleton.h"
#include "engine/modelLookup.h"
#include <engine/resourceManager.h>
#include "engine/core/types.h"

World myWorld;
ResourceManager* RMan;
unsigned int shaderProgram;
auto lastTime = std::chrono::high_resolution_clock::now();
auto currentTime = std::chrono::high_resolution_clock::now();
std::chrono::duration<float> deltaTime;

void render(entt::registry& registry);
#ifdef __EMSCRIPTEN__

EMSCRIPTEN_WEBGL_CONTEXT_HANDLE context;

void main_loop() {
	extern World myWorld; // Use extern to reference a globally defined object
	emscripten_webgl_make_context_current(context);
	glClearColor(0.5, 0.5, 0.5, 1); // Clear the canvas with a grey color
	glClear(GL_COLOR_BUFFER_BIT);

	currentTime = std::chrono::high_resolution_clock::now();

	deltaTime = currentTime - lastTime;
	lastTime = currentTime;

	// Now render the triangle
	
	glDrawArrays(GL_TRIANGLES, 0, 3);

	myWorld.update(deltaTime.count());
}

#else

// Function to initialize GLFW and create a window
GLFWwindow* initWindow(const int width, const int height, const char* title) {
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

// Main rendering loop
void mainLoop(GLFWwindow* window) {
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
#endif 

void render(entt::registry& registry) {
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


void init() {
	// Create and compile the vertex shader
	const char* vertexShaderSource = "#version 300 es\n"
		"layout (location = 0) in vec3 aPos;\n"
		"void main()\n"
		"{\n"
		"   gl_Position = vec4(aPos.x, aPos.y, aPos.z, 1.0);\n"
		"}\0";

	const char* fragmentShaderSource = "#version 300 es\n"
		"precision mediump float;\n"
		"out vec4 FragColor;\n"
		"void main()\n"
		"{\n"
		"   FragColor = vec4(1.0f, 1.0f, 0.2f, 1.0f);\n"
		"}\0";

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

	Entity myEntity = myWorld.create_entity("test");

	//myEntity.add_component<VelocityComponent>();
	//myEntity.add_component<PositionComponent>(0.0f, 10.0f, 0.0f);
	//myEntity.add_component<AccelerationComponent>();
	myEntity.add_component<engine::StaticMeshComponent>(1);
	std::cout << "Hello, world!" << std::endl;
}

int main() {





#ifdef __EMSCRIPTEN__

	

	EmscriptenWebGLContextAttributes attrs;
	emscripten_webgl_init_context_attributes(&attrs);

	attrs.alpha = EM_FALSE;
	attrs.depth = EM_TRUE;
	attrs.stencil = EM_FALSE;
	attrs.antialias = EM_TRUE;
	attrs.preserveDrawingBuffer = EM_FALSE;
	attrs.enableExtensionsByDefault = EM_TRUE;
	attrs.premultipliedAlpha = EM_FALSE;
	attrs.majorVersion = 2; // Requesting WebGL 2 context
	attrs.minorVersion = 0;

	context = emscripten_webgl_create_context("#canvas", &attrs);
	if (!context) {
		std::cerr << "Failed to initialize WebGL context!" << std::endl;
		return -1;
	}
	emscripten_webgl_make_context_current(context);

	init();
	emscripten_set_main_loop(main_loop, 0, 1);
#else
	const int width = 800;
	const int height = 600;

	GLFWwindow* window = initWindow(width, height, "GLFW Window Example");
	if (!window) return -1;

	init();
	mainLoop(window);

	// Clean up and exit
	glfwDestroyWindow(window);
	glfwTerminate();
#endif

	return 0;
}
