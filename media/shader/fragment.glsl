#version 300 es
precision highp float;
in vec2 texCoord;

out vec4 color;
uniform sampler2D texture0;
uniform sampler2D texture1;
void main() {
    color = texture(texture1, texCoord);
}
