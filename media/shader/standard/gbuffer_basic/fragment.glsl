#version 330 core 
out vec4 out_color;

in vec3 vertexPosition;
in vec3 vertexNormal;
in vec2 vertexCoord;

uniform sampler2D diffuse_texture;
uniform sampler2D height_texture;
uniform sampler2D specular_texture;

struct AmbientLight {
    float strength;
    vec3 color;
};

struct PointLight {
    vec3 position;
    vec3 color;
};

uniform AmbientLight ambientLight;

uniform PointLight pointLights[32];

vec3 processPointLight(PointLight light) {
    vec3 lightDirection = normalize(light.position - vertexPosition);
    vec3 normal = normalize(vertexNormal);
    float diff = max(dot(normal, lightDirection), 0.0);
    return diff * light.color;
}

void main() {
    out_color = texture2D(diffuse_texture, vertexCoord);
    vec3 lightColor;

    vec3 ambientColor = ambientLight.strength * ambientLight.color;
    lightColor = ambientColor;
    #if NUM_POINT_LIGHT > 0
    for(int i = 0; i < NUM_POINT_LIGHT; i++) {
        lightColor += processPointLight(pointLights[i]);
    }
    #endif

    out_color = vec4(lightColor, 1.0) * out_color;
}