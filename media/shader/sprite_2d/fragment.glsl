#version 330 core 
in vec2 coord;
out vec4 out_color;
uniform sampler2D texture_0;
void main() {
    out_color = texture(texture_0, coord);
}