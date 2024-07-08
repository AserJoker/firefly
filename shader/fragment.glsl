#version 300 es
precision highp float;
out vec4 color;
uniform uint s_Time;
void main() {
    float s_color = float(s_Time);
    s_color = (sin(s_color * 0.001f) + 1.0f) / 2.0f;
    color = vec4(s_color, 0, 0, 1.0f);
}
