#version 330 core
layout(location = 0) in vec2 position;
layout(location = 3) in vec2 texcoord;

out vec2 vertexTexcoord;

uniform mat4 projection;
uniform mat4 model;
uniform mat4 diffuse_texture_coord_matrix;

void main() {
    vertexTexcoord = (diffuse_texture_coord_matrix * vec4(texcoord, 0.0, 1.0)).xy;
    gl_Position = projection * model * vec4(position, 0.0, 1.0);
}