#version 330 core

in vec2 vertexTextureCoord;
uniform sampler2D diffuse;

void main() {
    gl_FragColor = texture(diffuse, vertexTextureCoord);
}
