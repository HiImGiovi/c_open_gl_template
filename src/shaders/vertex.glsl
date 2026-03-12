#version 330 core
uniform mat4 MVP;
layout(location=0) in vec2 vPos;
layout(location=1) in vec3 vCol;
layout(location=2) in vec2 vUv;

out vec3 vColor;
out vec2 vTextureUv;

void main() {
    gl_Position = MVP * vec4(vPos, 0.0, 1.0);
    vColor = vCol;
    vTextureUv = vUv;
}