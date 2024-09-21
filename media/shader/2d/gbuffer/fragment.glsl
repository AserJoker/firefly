#version 330 core

in vec2 vertexTexcoord;

uniform sampler2D diffuse_texture;

void main() {
    vec4 color  = texture(diffuse_texture, vertexTexcoord);
    gl_FragColor = color;
}
