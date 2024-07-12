#version 300 es
precision highp float;
in vec2 texCoord;
in vec3 Normal;
in vec3 FragPos;

out vec4 color;
struct Light {
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
    vec3 position;
};
struct Material {
    sampler2D diffuse;
    sampler2D specular;
    float shininess;
};
uniform vec3 viewPos;
uniform Light light;
uniform Material material;
void main() {
    vec3 ambient = light.ambient * texture(material.diffuse, texCoord).rgb;
    // diffuse 
    vec3 norm = normalize(Normal);
    vec3 lightDir = normalize(light.position - FragPos);
    float diff = max(dot(norm, lightDir), 0.0f);
    vec3 diffuse = light.diffuse * diff * texture(material.diffuse, texCoord).rgb;  

    // specular
    vec3 viewDir = normalize(viewPos - FragPos);
    vec3 reflectDir = reflect(-lightDir, norm);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0f), material.shininess);
    vec3 specular = light.specular * spec * texture(material.specular, texCoord).rgb;

    vec3 result = ambient + diffuse + specular;
    color = vec4(result, 1.0f);
}
