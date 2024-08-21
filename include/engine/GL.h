#ifdef __EMSCRIPTEN__
	#include <emscripten.h>
	#include <emscripten/html5.h>
	#include <GLES3/gl3.h>
#else
	#include <glad/glad.h>
	#include <GLFW/glfw3.h>
#endif 

