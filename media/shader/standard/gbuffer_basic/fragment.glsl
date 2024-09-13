#version 330 core 
out vec4 out_color;

in vec3 vertexPosition;
in vec3 vertexNormal;
in vec2 vertexCoord;

uniform sampler2D diffuse_texture;
uniform sampler2D specular_texture;

struct AmbientLight {
    float strength;
    vec3 color;
    vec3 result;
};

struct PointLight {
    vec3 position;
    vec3 color;
};

uniform uint numPointLight;

uniform AmbientLight ambientLight;

uniform vec3 cameraPosition;

uniform PointLight pointLights[32];

float specularStrength = 0.5;

uniform int time;

void main() {

    vec3 diff_color = vec3(texture2D(diffuse_texture, vertexCoord));
    vec3 specular_color = vec3(texture2D(specular_texture, vertexCoord));

    out_color = vec4(0.0);

    vec3 ambient = ambientLight.result * diff_color;

    for(uint i = uint(0); i < numPointLight; i++) {
        vec3 lightColor = pointLights[i].color;
        vec3 lightPosition = vec3(sin(float(time) * 0.01), 0, cos(float(time) * 0.01)) * 10;

        vec3 normal = normalize(vertexNormal);
        vec3 lightDir = normalize(lightPosition - vertexPosition);
        float diff = max(dot(normal, lightDir), 0.0);
        vec3 diffuse = diff * lightColor * diff_color;

        vec3 viewDir = normalize(cameraPosition - vertexPosition);
        vec3 reflectDir = reflect(-lightDir, normal);
        float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32);
        vec3 specular = specularStrength * spec * lightColor * specular_color;

        out_color += vec4(diffuse + ambient + specular, 1.0);
    }
}
