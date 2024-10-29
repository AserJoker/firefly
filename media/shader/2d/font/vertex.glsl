#version 330 core
layout(location = 0) in vec3 position;
layout(location = 1) in vec2 coord;
layout(location = 2) in mat4 instance_model;
layout(location = 6) in mat4 instance_tex_coord_model;
layout(location = 10) in vec4 color;

uniform mat4 projection;
uniform mat4 view;

out vec2 vertex_texture_coord;
out mat4 vertex_instanced_coord_model;
out vec4 vertex_color;

void main() {
    gl_Position = projection * view * instance_model * vec4(position, 1.0);
    vertex_texture_coord = coord;
    vertex_instanced_coord_model = instance_tex_coord_model;
    vertex_color = color;
}