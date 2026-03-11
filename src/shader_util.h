#ifndef SHADER_UTIL_H
#define SHADER_UTIL_H

#include <glad/glad.h>

GLuint load_shader_program(const char *vertex_path, const char *fragment_path, const char *geometry_path);

#endif