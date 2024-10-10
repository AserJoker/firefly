#version 330 core

in vec2 vertexTexcoord;
in vec3 vertexColor;

uniform sampler2D diffuse_texture;
uniform float diffuse_texture_blend;
uniform int diffuse_texture_enable;

void main() {
    if(diffuse_texture_enable == 1) {
        vec4 color = texture(diffuse_texture, vertexTexcoord) * vec4(vertexColor, 1.0);
        color.a *= diffuse_texture_blend;
        gl_FragColor = color;
    } else {
        gl_FragColor = vec4(vertexColor,1.0f);
    }
}
