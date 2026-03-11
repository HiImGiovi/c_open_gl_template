#include <stddef.h>
#include <stdio.h>
#include <math.h>
#include "renderer.h"
#include "shader_util.h"

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

void renderer_push_quad(Renderer *r, vec2 min, vec2 max, vec3 color)
{
    uint32_t base = r->vertex_count;

    renderer_push_vertex(r, (Vertex){.pos = {min[0], min[1]}, .col = {color[0], color[1], color[2]}});
    renderer_push_vertex(r, (Vertex){.pos = {max[0], min[1]}, .col = {color[0], color[1], color[2]}});
    renderer_push_vertex(r, (Vertex){.pos = {max[0], max[1]}, .col = {color[0], color[1], color[2]}});
    renderer_push_vertex(r, (Vertex){.pos = {min[0], max[1]}, .col = {color[0], color[1], color[2]}});

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

    glEnableVertexAttribArray(r->vpos_location);
    glVertexAttribPointer(r->vpos_location, 2, GL_FLOAT, GL_FALSE,
                          sizeof(Vertex), (void *)offsetof(Vertex, pos));
    glEnableVertexAttribArray(r->vcol_location);
    glVertexAttribPointer(r->vcol_location, 3, GL_FLOAT, GL_FALSE,
                          sizeof(Vertex), (void *)offsetof(Vertex, col));
}

void renderer_draw(Renderer *r, int frame_buffer_width, int frame_buffer_height)
{
    float ratio = frame_buffer_width / (float)frame_buffer_height;

    mat4x4 m, p, mvp;
    mat4x4_identity(m);
    // mat4x4_rotate_Z(m, m, (float)glfwGetTime());
    mat4x4_ortho(p, -ratio, ratio, -1.f, 1.f, -1.f, 1.f);
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
    glDrawElements(
        GL_TRIANGLES,
        r->index_count,
        GL_UNSIGNED_INT,
        0);
}
