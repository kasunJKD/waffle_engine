#version 330 core
out vec4 FragColor;

uniform vec3 objectColor;
uniform vec3 lightColor;
uniform int isLight;

void main() {
    if (isLight == 1) {
        FragColor = vec4(1.0);
    } else {
        FragColor = vec4(lightColor * objectColor, 1.0);
    }
}
