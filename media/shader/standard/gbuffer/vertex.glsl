#version 330 core 
layout(location = 0) in vec3 position;
layout(location = 1) in vec3 normal;
layout(location = 3) in vec2 coord;
layout(location = 11) in vec3 tangent;
layout(location = 12) in vec3 bitangent;

out vec3 vertexPosition;
out vec3 vertexNormal;
out vec2 vertexTexcoord;
out vec3 vertexTangent;
out vec3 vertexBitangent;
out mat3 TBN;

uniform mat4 projection;
uniform mat4 view;
uniform mat4 model;

void main() {
    gl_Position = projection * view * model * vec4(position, 1.0);
    vertexPosition = position;
    vertexTexcoord = coord;
    vec3 T = normalize(vec3(model * vec4(tangent, 0.0)));
    vec3 N = normalize(vec3(model * vec4(normal, 0.0)));
    vec3 B = cross(T, N);
    TBN = mat3(T, B, N);
    vertexTangent = tangent;
    vertexBitangent = bitangent;
}