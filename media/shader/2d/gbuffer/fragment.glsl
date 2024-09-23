#version 330 core

in vec2 vertexTexcoord;

uniform sampler2D diffuse_texture;
uniform float diffuse_texture_blend;
uniform mat4 diffuse_texture_coord_matrix;

void main() {
    vec4 coord = diffuse_texture_coord_matrix * vec4(vertexTexcoord, 0.0, 1.0);
    vec4 color = texture(diffuse_texture, coord.xy);
    gl_FragColor = color * diffuse_texture_blend;
}
