#version 330 core 
layout(location = 0) in vec3 position;
layout(location = 1) in vec3 normal;
layout(location = 3) in vec2 coord;

out vec3 vertexPosition;
out vec3 vertexNormal;
out vec2 vertexCoord;

uniform mat4 projection;
uniform mat4 view;
uniform mat4 model;

void main() {
    gl_Position = projection * view * model * vec4(position, 1.0);
    vertexPosition = position;
    vertexNormal = normal;
    vertexCoord = coord;
}