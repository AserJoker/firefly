#version 330 core 
layout(location = 0) in vec3 pos;
layout(location = 1) in vec2 tex_coord;
out vec2 coord;
uniform mat4 model;
uniform mat4 uv_model;
uniform vec2 viewport_size;
void main() {
    vec4 pos = model * vec4(pos.x, pos.y, 0, 1.0);
    gl_Position = vec4(pos.x / viewport_size.x - 1, 1 - pos.y / viewport_size.y, 0, 1);
    coord = (uv_model * vec4(tex_coord.x, tex_coord.y, 0, 1.0)).xy;
}