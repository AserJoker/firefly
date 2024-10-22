#version 330 core

in vec2 vertexTextureCoord;
uniform sampler2D diffuse_texture;
uniform mat4 diffuse_matrix;

void main() {
    gl_FragColor = texture(diffuse_texture, (diffuse_matrix * vec4(vertexTextureCoord, 0.0, 1.0)).xy);
}
