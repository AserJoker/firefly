#version 330 core
out vec4 color;
uniform int s_Time;
void main() {
    color = vec4((sin(s_Time * 0.001) + 1) / 2, 0, 0, 1.0);
}
