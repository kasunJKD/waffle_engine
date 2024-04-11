#version 460 core
out vec4 FragColor;

in vec2 TexCoord;

void main()
{
    vec3 topColor = vec3(1.0, 0.0, 0.0); // Red
    vec3 bottomColor = vec3(0.0, 0.0, 1.0); // Blue
    float mixFactor = TexCoord.y;
    vec3 color = mix(bottomColor, topColor, mixFactor);
    FragColor = vec4(color, 1.0);
}
