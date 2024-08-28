#version 330 core 
out vec4 out_color;
in vec3 outColor;
in vec2 outCoord;
uniform sampler2D texture0;
void main() {
    out_color = texture2D(texture0, outCoord);
}