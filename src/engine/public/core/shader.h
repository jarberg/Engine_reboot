// Shader.h
#pragma once
#include <string>
#include <unordered_map>
#include <stdexcept>

#include "core/fileLoader.h"
#include "engine/core/GL.h"

class Shader {
public:
    explicit Shader(GLuint program = 0) : program_(program) {}
    ~Shader() { if (program_) glDeleteProgram(program_); }

    Shader(const Shader&) = delete;
    Shader& operator=(const Shader&) = delete;

    Shader(Shader&& o) noexcept : program_(o.program_), cache_(std::move(o.cache_)) { o.program_ = 0; }
    Shader& operator=(Shader&& o) noexcept {
        if (this != &o) { if (program_) glDeleteProgram(program_); program_ = o.program_; cache_ = std::move(o.cache_); o.program_ = 0; }
        return *this;
    }

    static Shader fromFiles(const std::string vsPath, const std::string fsPath);
    void use() const { glUseProgram(program_); }

    GLint uniform(const char* name) {
        if (auto it = cache_.find(name); it != cache_.end()) return it->second;
        GLint loc = glGetUniformLocation(program_, name);
        cache_[name] = loc;
        return loc;
    }

    GLuint id() const { return program_; }

private:
    GLuint program_{ 0 };
    std::unordered_map<std::string, GLint> cache_;
};
