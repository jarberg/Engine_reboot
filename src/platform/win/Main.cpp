
#include <core/Main.h>
#include <core/input.h>

#include <vector>

std::vector<int> myVector = { 0, 0, 0};
World* myWorld = new World();

extern void initInputHandlers(WindowHandle window);

static void OnKey(WindowHandle, int jsKey, int scancode, KeyAction action, std::uint16_t mods) {
	InputHandler::setKeyState(jsKey, action);
	(void)scancode; (void)mods;
}

GLFWwindow* initWindow(int width, int height, const char* title) {
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

		glClearColor(0.5f, 0.5f, 0.5f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		InputHandler::fireHeldPressed();

		render(myWorld->get_registry());

		glfwSwapBuffers(window);
		glfwPollEvents();
		myWorld->update();
	}
	return 0;
}

int main() {
	int widthInt = static_cast<int>(width);
	int heightInt = static_cast<int>(height);

	GLFWwindow* window = initWindow(width, height, "GLFW Window Example");
	if (!window) return -1;
	
	glfwGetWindowSize(window, &widthInt, &heightInt); 

	myWorld->makeCurrent();

	WsClient::GetInstance();

	init();

	initInputHandlers(static_cast<WindowHandle>(window));

	mainLoop(window);

	glfwDestroyWindow(window);
	glfwTerminate();

	return 0;
}

