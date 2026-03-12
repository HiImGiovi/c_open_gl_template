
#ifndef RENDERER_H
#define RENDERER_H

#include <glad/glad.h>
#include <linmath.h>
#define GLAD_GL_IMPLEMENTATION

#define VERTEX_BUFFER_CAP 400000
#define INDEX_BUFFER_CAP (VERTEX_BUFFER_CAP * 2)

typedef struct Vertex
{
    vec2 pos;
    vec3 col;
    vec2 uv;
} Vertex;

typedef struct Renderer
{
    GLuint vao;
    GLuint vbo;
    GLuint ebo;
    GLuint program;
    GLuint active_texture;
    GLint mvp_uniform_location;
    GLint vpos_location;
    GLint vcol_location;
    GLint vuv_location;
    Vertex vertex_buffer[VERTEX_BUFFER_CAP];
    uint32_t index_buffer[INDEX_BUFFER_CAP];
    size_t vertex_count;
    size_t index_count;

} Renderer;

typedef struct
{
    GLuint id;
    int width;
    int height;
} Texture;

void renderer_init(Renderer *r);
void renderer_push_vertex(Renderer *r, Vertex v);
void renderer_push_triangle(Renderer *r, vec2 a, vec2 b, vec2 c, vec3 color);
void renderer_push_quad(Renderer *r, vec2 pos, vec2 size, vec3 color);
void renderer_draw(Renderer *r, int frame_buffer_width, int frame_buffer_height);
Texture renderer_load_texture(const char *path);
void renderer_push_sprite(Renderer *r, Texture texture, vec2 pos);

#endif