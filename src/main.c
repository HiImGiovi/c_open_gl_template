
#define GLAD_GL_IMPLEMENTATION
#include <glad/glad.h>
#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>
#include <stdlib.h>
#include <stddef.h>
#include <stdio.h>
#include "renderer.h"

static const Vertex vertices[3] = {
    {{-0.6f, -0.4f}, {1.f, 0.f, 0.f}},
    {{0.6f, -0.4f}, {0.f, 1.f, 0.f}},
    {{0.f, 0.6f}, {0.f, 0.f, 1.f}},
};

static void error_callback(int error, const char *description)
{
    fprintf(stderr, "Error: %s\n", description);
}

static void key_callback(GLFWwindow *window, int key, int scancode, int action, int mods)
{
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
        glfwSetWindowShouldClose(window, GLFW_TRUE);
}

static Renderer global_renderer = {0};

int main(void)
{
    glfwSetErrorCallback(error_callback);

    if (!glfwInit())
        exit(EXIT_FAILURE);

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow *window = glfwCreateWindow(640, 480, "OpenGL Triangle", NULL, NULL);
    if (!window)
    {
        glfwTerminate();
        exit(EXIT_FAILURE);
    }

    glfwSetKeyCallback(window, key_callback);

    glfwMakeContextCurrent(window);
    gladLoadGL();
    glfwSwapInterval(1);

    Renderer *r = &global_renderer;
    renderer_init(r);
    // for (size_t i = 0; i < 3; i++)
    // {
    //     renderer_push_vertex(r, vertices[i]);
    // }
    vec2 min = {-0.5f, -0.5f};
    vec2 max = {0.5f, 0.5f};

    vec3 blue = {0, 0, 1};

    renderer_push_quad(r, min, max, blue);
    while (!glfwWindowShouldClose(window))
    {
        int width, height;
        glfwGetFramebufferSize(window, &width, &height);

        glViewport(0, 0, width, height);
        glClear(GL_COLOR_BUFFER_BIT);

        renderer_draw(r, width, height);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwDestroyWindow(window);

    glfwTerminate();
    exit(EXIT_SUCCESS);
}