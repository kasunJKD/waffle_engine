#version 330 core
in vec2 TexCoords;
out vec4 color;
uniform sampler2D textAtlas;
uniform vec3 textColor;

void main() {
    float alpha = texture(textAtlas, TexCoords).r; // Ensure red channel is used
    if (alpha < 0.1) discard; // Avoid rendering invisible pixels
    color = vec4(textColor, alpha);
}
