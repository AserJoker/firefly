#version 330 core
layout(location = 0) in vec2 position;
layout(location = 3) in vec2 texcoord;
layout(location = 4) in mat4 view;
out vec2 vertexTexcoord;

uniform mat4 projection;
uniform mat4 model;
uniform mat4 diffuse_texture_coord_matrix;

uniform int flip;

void main() {
    vertexTexcoord = (diffuse_texture_coord_matrix * vec4(texcoord.x, texcoord.y, 0.0, 1.0)).xy;
    vec4 pos = projection * view * model * vec4(position.x, position.y, 0.0, 1.0);
    if(flip == 1) {
        gl_Position = vec4(pos.x - 1.0, pos.y - 1.0, pos.zw);
    } else {
        gl_Position = vec4(pos.x - 1.0, 1 - pos.y, pos.zw);
    }
}