#version 330 core
layout(location = 0) in vec2 position;
layout(location = 3) in vec2 texcoord;
layout(location = 4) in mat4 view;
layout(location = 8) in mat4 coord_matrix;
layout(location = 12) in vec3 color;

out vec2 vertexTexcoord;
out vec3 vertexColor;

uniform mat4 projection;
uniform mat4 model;
uniform vec3 cameraPosition;

uniform int flip;

void main() {
    vertexTexcoord = (coord_matrix * vec4(texcoord.x, texcoord.y, 0.0, 1.0)).xy;
    vec4 pos = view * model * vec4(position.x, position.y, 0.0, 1.0);
    pos.x += cameraPosition.x;
    pos.y += cameraPosition.y;
    pos = projection * pos;
    if(flip == 1) {
        gl_Position = vec4(pos.x - 1.0, pos.y - 1.0, pos.zw);
    } else {
        gl_Position = vec4(pos.x - 1.0, 1 - pos.y, pos.zw);
    }
    vertexColor = color;
}