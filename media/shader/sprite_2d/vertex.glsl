#version 330 core
layout(location = 0) in vec3 position;
layout(location = 3) in vec2 coord;

out vec2 outCoord;

uniform mat4 projection;
uniform mat4 view;

void main() {
    gl_Position = projection * view * vec4(position, 1.0);
    outCoord = coord;
}