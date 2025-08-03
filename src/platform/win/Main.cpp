
#include <core/Main.h>
#include <core/input.h>

#include <vector>

std::vector<int> myVector = { 0, 0, 0};
World* myWorld = nullptr;


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

int mainLoop(GLFWwindow* window) {
	
	while (!glfwWindowShouldClose(window)) {
		currentTime = std::chrono::high_resolution_clock::now();

		deltaTime = currentTime - lastTime;
		lastTime = currentTime;

		// Clear the screen to black
		glClearColor(0.5f, 0.5f, 0.5f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);

		pollInput();
		

		render(myWorld->get_registry());

		// Swap buffers and poll IO events
		glfwSwapBuffers(window);
		glfwPollEvents();
		myWorld->update(deltaTime.count());
	}
	return 0;
}

int main() {


	const int width = 800;
	const int height = 600;

	GLFWwindow* window = initWindow(width, height, "GLFW Window Example");
	if (!window) return -1;

	init();
	initInputHandlers();


	mainLoop(window);

	// Clean up and exit
	glfwDestroyWindow(window);
	glfwTerminate();


	return 0;
}

