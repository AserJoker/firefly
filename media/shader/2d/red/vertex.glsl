#version 330 core
layout(location = 0) in vec3 position;
layout(location = 1) in vec2 coord;

uniform mat4 projection;
uniform mat4 model;
uniform mat4 view;

out vec2 vertex_texture_coord;

void main() {
    gl_Position = projection * view * model * vec4(position, 1.0);
    vertex_texture_coord = coord;
}