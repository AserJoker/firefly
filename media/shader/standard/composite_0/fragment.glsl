#version 330 core
out vec4 out_color;

in vec2 vertexTexcoord;

uniform sampler2D attachment_0;

void main() {
    vec3 col = texture(attachment_0, vertexTexcoord).rgb;
    out_color = vec4(col, 1.0);
}
