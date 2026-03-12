#include <stddef.h>
#include <stdio.h>
#include <math.h>
#include "renderer.h"
#include "shader_util.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

void renderer_push_vertex(Renderer *r, Vertex v)
{
    if (r->vertex_count >= VERTEX_BUFFER_CAP)
    {
        // TODO: handling realloc or artenatively trigger a draw call
        return;
    }
    memcpy(&r->vertex_buffer[r->vertex_count], &v, sizeof(Vertex));
    r->vertex_count++;
}

void renderer_push_triangle(Renderer *r, vec2 a, vec2 b, vec2 c, vec3 color)
{
    uint32_t base = r->vertex_count;

    renderer_push_vertex(r, (Vertex){.pos = {a[0], a[1]}, .col = {color[0], color[1], color[2]}});
    renderer_push_vertex(r, (Vertex){.pos = {b[0], b[1]}, .col = {color[0], color[1], color[2]}});
    renderer_push_vertex(r, (Vertex){.pos = {c[0], c[1]}, .col = {color[0], color[1], color[2]}});

    r->index_buffer[r->index_count++] = base + 0;
    r->index_buffer[r->index_count++] = base + 1;
    r->index_buffer[r->index_count++] = base + 2;
}

/// draw this
///      size[0]
///    3----------2
///    |          |
///    |          | size[1]
///    |          |
///    0----------1
///  point 0 has coordinate x=pos[0], y=pos[1]
void renderer_push_quad(Renderer *r, vec2 pos, vec2 size, vec3 color)
{
    uint32_t base = r->vertex_count;

    renderer_push_vertex(r, (Vertex){.pos = {pos[0], pos[1]}, .col = {color[0], color[1], color[2]}, .uv = {0.0f, 0.0f}});                     // 0
    renderer_push_vertex(r, (Vertex){.pos = {pos[0] + size[0], pos[1]}, .col = {color[0], color[1], color[2]}, .uv = {1.0f, 0.0f}});           // 1
    renderer_push_vertex(r, (Vertex){.pos = {pos[0] + size[0], pos[1] + size[1]}, .col = {color[0], color[1], color[2]}, .uv = {1.0f, 1.0f}}); // 2
    renderer_push_vertex(r, (Vertex){.pos = {pos[0], pos[1] + size[1]}, .col = {color[0], color[1], color[2]}, .uv = {0.0f, 1.0f}});           // 3

    r->index_buffer[r->index_count++] = base + 0;
    r->index_buffer[r->index_count++] = base + 1;
    r->index_buffer[r->index_count++] = base + 2;

    r->index_buffer[r->index_count++] = base + 0;
    r->index_buffer[r->index_count++] = base + 2;
    r->index_buffer[r->index_count++] = base + 3;
}

void renderer_init(Renderer *r)
{
    r->vertex_count = 0;
    r->index_count = 0;
    glGenVertexArrays(1, &r->vao);
    glBindVertexArray(r->vao);

    glGenBuffers(1, &r->vbo);
    glBindBuffer(GL_ARRAY_BUFFER, r->vbo);

    glGenBuffers(1, &r->ebo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, r->ebo);

    r->program = load_shader_program("src/shaders/vertex.glsl", "src/shaders/fragment.glsl", NULL);
    r->mvp_uniform_location = glGetUniformLocation(r->program, "MVP");
    r->vpos_location = 0;
    r->vcol_location = 1;
    r->vuv_location = 2;

    glEnableVertexAttribArray(r->vpos_location);
    glVertexAttribPointer(r->vpos_location, 2, GL_FLOAT, GL_FALSE,
                          sizeof(Vertex), (void *)offsetof(Vertex, pos));
    glEnableVertexAttribArray(r->vcol_location);
    glVertexAttribPointer(r->vcol_location, 3, GL_FLOAT, GL_FALSE,
                          sizeof(Vertex), (void *)offsetof(Vertex, col));
    glEnableVertexAttribArray(r->vuv_location);
    glVertexAttribPointer(r->vuv_location, 2, GL_FLOAT, GL_FALSE,
                          sizeof(Vertex), (void *)offsetof(Vertex, uv));
}

void renderer_draw(Renderer *r, int frame_buffer_width, int frame_buffer_height)
{
    float ratio = frame_buffer_width / (float)frame_buffer_height;

    mat4x4 m, p, mvp;
    mat4x4_identity(m);
    // mat4x4_rotate_Z(m, m, (float)glfwGetTime());
    mat4x4_ortho(p, 0.0f, frame_buffer_width,
                 frame_buffer_height, 0.0f,
                 -1.0f, 1.0f);
    mat4x4_mul(mvp, p, m);

    glBindBuffer(GL_ARRAY_BUFFER, r->vbo);
    glBufferData(GL_ARRAY_BUFFER, r->vertex_count * sizeof(Vertex), r->vertex_buffer, GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, r->ebo);
    glBufferData(
        GL_ELEMENT_ARRAY_BUFFER,
        r->index_count * sizeof(uint32_t),
        r->index_buffer,
        GL_STATIC_DRAW);

    glUseProgram(r->program);
    glUniformMatrix4fv(r->mvp_uniform_location, 1, GL_FALSE, (const GLfloat *)&mvp);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, r->active_texture);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glDrawElements(
        GL_TRIANGLES,
        r->index_count,
        GL_UNSIGNED_INT,
        0);
}

void renderer_push_sprite(Renderer *r, Texture texture, vec2 pos)
{
    r->active_texture = texture.id;
    vec2 size = {texture.width, texture.height};
    renderer_push_quad(r, pos, size, (vec3){1.0f, 1.0f, 1.0f});
}

Texture renderer_load_texture(const char *path)
{
    int width, height, channels;

    unsigned char *data = stbi_load(path, &width, &height, &channels, 4);

    if (!data)
    {
        printf("Failed to load texture: %s\n", path);
        exit(EXIT_FAILURE);
    }

    GLuint texture;
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    glTexImage2D(
        GL_TEXTURE_2D,
        0,
        GL_RGBA,
        width,
        height,
        0,
        GL_RGBA,
        GL_UNSIGNED_BYTE,
        data);

    glGenerateMipmap(GL_TEXTURE_2D);

    stbi_image_free(data);

    return (Texture){
        .id = texture,
        .width = width,
        .height = height,
    };
}
