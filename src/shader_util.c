
#include "shader_util.h"
#include "file_io.h"
#include <stdio.h>
#include <stdlib.h>

static void check_compile(GLuint shader)
{
    GLint success;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        char info[512];
        glGetShaderInfoLog(shader, 512, NULL, info);
        fprintf(stderr, "Shader compilation failed:\n%s\n", info);
    }
}

static void check_link(GLuint program)
{
    GLint success;
    glGetProgramiv(program, GL_LINK_STATUS, &success);
    if (!success)
    {
        char info[512];
        glGetProgramInfoLog(program, 512, NULL, info);
        fprintf(stderr, "Program linking failed:\n%s\n", info);
    }
}

GLuint load_shader_program(const char *vertex_path,
                           const char *fragment_path,
                           const char *geometry_path)
{
    char *vertex_src = read_file(vertex_path);
    char *fragment_src = read_file(fragment_path);
    char *geometry_src = NULL;

    GLuint vertex_shader = 0;
    GLuint fragment_shader = 0;
    GLuint geometry_shader = 0;

    vertex_shader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertex_shader, 1, (const char **)&vertex_src, NULL);
    glCompileShader(vertex_shader);
    check_compile(vertex_shader);

    fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragment_shader, 1, (const char **)&fragment_src, NULL);
    glCompileShader(fragment_shader);
    check_compile(fragment_shader);

    if (geometry_path != NULL)
    {
        geometry_src = read_file(geometry_path);

        if (geometry_src != NULL)
        {
            geometry_shader = glCreateShader(GL_GEOMETRY_SHADER);
            glShaderSource(geometry_shader, 1, (const char **)&geometry_src, NULL);
            glCompileShader(geometry_shader);
            check_compile(geometry_shader);
        }
    }

    GLuint program = glCreateProgram();

    glAttachShader(program, vertex_shader);
    glAttachShader(program, fragment_shader);

    if (geometry_shader != 0)
        glAttachShader(program, geometry_shader);

    glLinkProgram(program);
    check_link(program);

    glDeleteShader(vertex_shader);
    glDeleteShader(fragment_shader);

    if (geometry_shader != 0)
        glDeleteShader(geometry_shader);

    free(vertex_src);
    free(fragment_src);

    if (geometry_src != NULL)
        free(geometry_src);

    return program;
}