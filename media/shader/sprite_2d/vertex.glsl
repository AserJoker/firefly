#version 330 core
in vec3 color;
in vec3 position;

out vec4 aColor;
void main() {
    gl_Position = vec4(position.x, position.y, position.z, 1.0);
    aColor = vec4(color, 1.0);
}