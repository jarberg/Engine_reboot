#ifdef __EMSCRIPTEN__
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

EMSCRIPTEN_WEBGL_CONTEXT_HANDLE context;


void mainLoop() {
	extern World myWorld; // Use extern to reference a globally defined object
	emscripten_webgl_make_context_current(context);
	glClearColor(0.5, 0.5, 0.5, 1); // Clear the canvas with a grey color
	glClear(GL_COLOR_BUFFER_BIT);

	currentTime = std::chrono::high_resolution_clock::now(); 

	deltaTime = currentTime - lastTime;
	lastTime = currentTime;

	// Now render the triangle
	render(myWorld.get_registry());

	myWorld.update(deltaTime.count());
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
	emscripten_webgl_make_context_current(context);

	init();

	emscripten_run_script("document.getElementById('canvas').setAttribute('tabindex', '0');");
	initInputHandlers();
	emscripten_set_main_loop(mainLoop, 0, 1);

	return 0;
}
#endif