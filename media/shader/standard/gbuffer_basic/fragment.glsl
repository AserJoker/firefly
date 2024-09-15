#version 330 core 
out vec4 out_color;

in vec3 vertexPosition;
in vec3 vertexNormal;
in vec2 vertexCoord;
in mat3 TBN;
in vec3 vertexTangent;
in vec3 vertexBitangent;

uniform sampler2D diffuse_texture;
uniform sampler2D specular_texture;
uniform sampler2D height_texture;

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
    vec3 normal_color = texture2D(height_texture, vertexCoord).rgb;
    vec3 normal = normalize(normal_color * 2.0 - 1.0);
    mat3 vTBN = transpose(TBN);
    out_color = vec4(0.0);
    vec3 viewPos = vTBN * cameraPosition;
    vec3 fragPos = vTBN * vertexPosition;
    vec3 viewDir = normalize(viewPos - fragPos);

    vec3 ambient = ambientLight.result * diff_color;

    for(uint i = uint(0); i < numPointLight; i++) {
        vec3 lightColor = pointLights[i].color;
        vec3 lightPosition = vTBN * pointLights[i].position;

        vec3 lightDir = normalize(lightPosition - fragPos);
        vec3 halfwayDir = normalize(lightDir + viewDir);

        float diff = max(dot(normal, lightDir), 0.0);
        vec3 diffuse = diff * lightColor * diff_color;

        float spec = pow(max(dot(normal, halfwayDir), 0.0), 32);
        vec3 specular = specularStrength * spec *specular_color;

        out_color += vec4(ambient + diffuse + specular, 1.0);
    }
}
