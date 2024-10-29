#version 330 core

in vec2 vertex_texture_coord;
in mat4 vertex_instanced_coord_model;
in vec4 vertex_color;
uniform sampler2D diffuse_texture;

void main() {
    float alpha = texture(diffuse_texture, (vertex_instanced_coord_model * vec4(vertex_texture_coord, 0.0, 1.0)).xy).r;
    gl_FragColor = vec4(vertex_color.rgb, vertex_color.a * alpha);
}
