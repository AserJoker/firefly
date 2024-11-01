#version 330 core
layout(location = 0) in vec3 position;
layout(location = 1) in vec2 coord;
layout(location = 2) in mat4 instancedModel;

uniform mat4 projection;
uniform mat4 view;
uniform mat4 model;
out vec2 vertex_texture_coord;

void main() {
    gl_Position = projection * view * model * instancedModel * vec4(position, 1.0);
    vertex_texture_coord = coord;
}