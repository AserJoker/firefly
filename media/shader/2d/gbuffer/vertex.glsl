#version 330 core
layout(location = 0) in vec2 position;
layout(location = 3) in vec2 texcoord;

out vec2 vertexTexcoord;

uniform mat4 projection;
uniform mat4 model;

void main() {
    vertexTexcoord = texcoord;
    gl_Position = projection * model vec4(position.x, position.y, 0.0, 1.0);
}