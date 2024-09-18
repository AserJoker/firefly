#version 330 core
out vec4 FragColor;

in vec2 TexCoords;

uniform sampler2D final_color;

void main()
{
    vec3 col = texture(final_color, TexCoords).rrr;
    FragColor = vec4(col, 1.0);
} 