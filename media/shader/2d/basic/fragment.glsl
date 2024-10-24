#version 330 core

in vec2 vertexTextureCoord;
uniform sampler2D diffuse_texture;
uniform int diffuse_enable;
uniform mat4 diffuse_matrix;

void main() {
    if(diffuse_enable == 1) {
        vec4 color = texture(diffuse_texture, (diffuse_matrix * vec4(vertexTextureCoord, 0.0, 1.0)).xy);
        gl_FragColor = color;
    } else {
        gl_FragColor = vec4(0, 0, 0, 1);
    }
}
