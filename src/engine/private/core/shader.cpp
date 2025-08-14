#include <core/shader.h>


static GLuint compile(GLenum type, const char* src) {
    GLuint s = glCreateShader(type);
    glShaderSource(s, 1, &src, nullptr);
    glCompileShader(s);
    GLint ok = 0; glGetShaderiv(s, GL_COMPILE_STATUS, &ok);
    if (!ok) { 
        char log[2048]; 
        glGetShaderInfoLog(s, 2048, nullptr, log); 
        glDeleteShader(s); 
        throw std::runtime_error(log); }
    return s;
}
static GLuint link(GLuint vs, GLuint fs) {
    GLuint p = glCreateProgram();
    glAttachShader(p, vs); glAttachShader(p, fs); glLinkProgram(p);
    GLint ok = 0; glGetProgramiv(p, GL_LINK_STATUS, &ok);
    glDeleteShader(vs); glDeleteShader(fs);
    if (!ok) { 
        char log[2048]; 
        glGetProgramInfoLog(p, 2048, NULL, log); 
        glDeleteProgram(p); 
        throw std::runtime_error(log); }
    return p;
}

Shader Shader::fromFiles(const std::string vsPath, const std::string fsPath) {
	const std::string vs = read_file<std::string>(vsPath);
	const std::string fs = read_file<std::string>(fsPath);

	int success;
	char infoLog[512];

	const unsigned int vertexShader = compile(GL_VERTEX_SHADER, vs.c_str());
	const unsigned int fragmentShader = compile(GL_FRAGMENT_SHADER, fs.c_str());

    return Shader(link(vertexShader, fragmentShader));
}
