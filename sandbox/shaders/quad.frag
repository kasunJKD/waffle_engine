#version 330 core
out vec4 FragColor;

uniform int isLight;

uniform vec3 viewPos;

struct Material {
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
    float shine;
};

struct Light {
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
    vec3 position;
};

uniform Material material;
uniform Light light;

in vec3 Normal;
in vec3 FragPos;

void main() {
    if (isLight == 1) {
        FragColor = vec4(1.0);
    } else {
        vec3 ambient = material.ambient * light.ambient;

        vec3 norm = normalize(Normal);
        vec3 lightDir = normalize(light.position - FragPos);
        float diff = max(dot(norm, lightDir), 0.0);
        vec3 diffuse = (diff * material.diffuse) * light.diffuse;

        vec3 viewDir = normalize(viewPos - FragPos);
        vec3 reflectDir = reflect(-lightDir, norm);
        float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shine);
        vec3 specular = (material.specular * spec) * light.specular; 

        vec3 result = ambient + diffuse + specular;
        FragColor = vec4(result, 1.0);
    }
}
