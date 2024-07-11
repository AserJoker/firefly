#version 300 es
precision highp float;
in vec2 texCoord;
in vec3 Normal;
in vec3 FragPos;

out vec4 color;
uniform sampler2D texture0;
uniform sampler2D texture1;
uniform vec3 lightPos;
uniform vec3 viewPos;
void main() {
    vec4 lightColor = vec4(1.0f, 1.0f, 1.0f, 1.0f);
    vec3 norm = normalize(Normal);
    vec3 lightDir = normalize(vec3(3, 2, -3) - FragPos);
    float diff = max(dot(norm, lightDir), 0.0f);
    vec4 diffuse = diff * lightColor;
    vec4 ambient = 0.2f * lightColor;
    float specularStrength = 0.5f;
    vec3 viewDir = normalize(viewPos - FragPos);
    vec3 reflectDir = reflect(-lightDir, norm);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0f), 32.0f);
    vec4 specular = specularStrength * spec * lightColor;
    color = (specular + ambient + diffuse) * texture(texture1, texCoord);
}
