#version 330 core 
out vec4 out_color;
in vec2 outCoord;
uniform sampler2D diffuse_texture;
void main() {
    out_color = texture2D(diffuse_texture, outCoord);
}