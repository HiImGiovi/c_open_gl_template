#version 330 core
in vec3 vColor;
in vec2 vTextureUv;
out vec4 fragColor;

uniform sampler2D sampler;

void main() {
    fragColor = texture(sampler, vTextureUv) ;
}