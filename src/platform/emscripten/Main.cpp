#include <core/Main.h>
#include <core/input.h>

#include "core/entity.h"
#include "core/world.h"
#include "core/fileLoader.h"
#include <core/resourceManager.h>
#include "core/singleton.h"
#include "core/modelLookup.h"
#include <core/types.h>
#include <core/extensions.h>

#ifdef __EMSCRIPTEN__


EMSCRIPTEN_WEBGL_CONTEXT_HANDLE context;

World* myWorld = nullptr;

void mainLoop() {
	
	emscripten_webgl_make_context_current(context);
	glClearColor(0.5, 0.5, 0.5, 1); 
	glClear(GL_COLOR_BUFFER_BIT);

	pollInput();
	myWorld->update();

	render(myWorld->get_registry());

	
}

int main(){
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
	else {
		std::cout << "WebGL 2 context created successfully." << std::endl;
	}
	std::cout << "before make context" << std::endl;
	emscripten_webgl_make_context_current(context);
	std::cout << "before init" << std::endl;

	myWorld = new World();
	createPerspectiveMatrix(45.0f * 3.14159f / 180.0f,  // 45� FOV
		1280 / 720, // aspect ratio
		0.1f,                       // near plane
		500.0f,                     // far plane
		projection);
	init();
	std::cout << "before run script" << std::endl;
	emscripten_run_script("document.getElementById('canvas').setAttribute('tabindex', '0');");
	EMSCRIPTEN_RESULT result = emscripten_set_canvas_element_size("#canvas", 1280, 720);
	if (result != EMSCRIPTEN_RESULT_SUCCESS) {
		printf("Failed to set canvas size.\n");
	}
	glViewport(0, 0, 1280, 720);
	
	initInputHandlers();
	emscripten_set_main_loop(mainLoop, 0, 1);

	return 0;
}
#endif