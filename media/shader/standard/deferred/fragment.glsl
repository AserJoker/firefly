#version 330 core

in vec2 vertexTexcoord;

uniform sampler2D attachment_0;

void main() {
    gl_FragColor = texture(attachment_0, vertexTexcoord);
}
