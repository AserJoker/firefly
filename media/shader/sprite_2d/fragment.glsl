#version 330 core 
out vec4 out_color;
in vec3 outColor;
in vec2 outCoord;
uniform sampler2D texture0;
layout(std140) uniform block{
    vec4 color;
};
void main() {
    out_color = color;
}