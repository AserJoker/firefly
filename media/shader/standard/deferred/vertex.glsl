#version 330 core
layout(location = 0) in vec2 position;
layout(location = 3) in vec2 texcoord;

out vec2 vertexTexcoord;

void main() {
    vertexTexcoord = texcoord;
    gl_Position = vec4(position, 0.0, 1.0);
}