#version 330 core

layout(lines_adjacency) in;
layout(triangle_strip, max_vertices = 4) out;

uniform float line_thickness;    
uniform vec2 viewport;

in vec3 vColor[];
out vec3 gColor;

void main()
{
    vec4 p1 = gl_in[1].gl_Position;
    vec4 p2 = gl_in[2].gl_Position;

    vec2 a = p1.xy / p1.w;
    vec2 b = p2.xy / p2.w;

    vec2 dir = normalize(b - a);

    vec2 normal = vec2(-dir.y, dir.x);

    vec2 pixelSize = 2.0 / viewport; 
    vec2 offset = normal * line_thickness * 0.5 * pixelSize;

    vec4 offset1 = vec4(offset * p1.w, 0.0, 0.0);
    vec4 offset2 = vec4(offset * p2.w, 0.0, 0.0);

    gColor = vColor[1];
    gl_Position = p1 + offset1;
    EmitVertex();

    gColor = vColor[1];
    gl_Position = p1 - offset1;
    EmitVertex();

    gColor = vColor[2];
    gl_Position = p2 + offset2;
    EmitVertex();

    gColor = vColor[2];
    gl_Position = p2 - offset2;
    EmitVertex();

    EndPrimitive();
}