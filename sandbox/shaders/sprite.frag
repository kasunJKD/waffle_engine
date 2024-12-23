#version 330 core

out vec4 FragColor;

in vec2 TexCoord;

uniform sampler2D texture1;
uniform vec4 texCoords; // Texture coordinates

void main() {
    vec2 adjustedTexCoord = vec2(
        mix(texCoords.x, texCoords.z, TexCoord.x),
        mix(texCoords.y, texCoords.w, TexCoord.y)
    );
    FragColor = texture(texture1, adjustedTexCoord);
//FragColor = vec4(TexCoord, 0.0, 1.0);
}
