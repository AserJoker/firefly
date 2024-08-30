#version 330 core 
out vec4 out_color;
in vec3 outColor;
in vec2 outCoord;
uniform sampler2D texture0;
uniform sampler2D texture1;
void main() {
    out_color = mix(texture2D(texture0, outCoord), texture2D(texture1, outCoord * 2), 0.5);
}