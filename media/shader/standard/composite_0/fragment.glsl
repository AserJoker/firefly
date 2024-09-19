#version 330 core

in vec2 vertexTexcoord;

uniform sampler2D attachment_0;

void main() {
    vec3 col = texture(attachment_0, vertexTexcoord).rgb;
    gl_FragColor = vec4(col, 1.0);
}
