#version 330 core

in vec2 vertexTexcoord;

uniform sampler2D diffuse_texture;
uniform float diffuse_texture_blend;

void main() {
    vec4 color = texture(diffuse_texture, vertexTexcoord);
    gl_FragColor = color * diffuse_texture_blend;
}