#version 330 core

in vec2 vertexTexcoord;

uniform sampler2D diffuse_texture;

void main() {
    vec3 col = texture(diffuse_texture, vertexTexcoord).rgb;
    gl_FragColor = vec4(col, 1.0);
}
