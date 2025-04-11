#version 330 core

//out vec4 FragColor;

//in vec2 TexCoord;

//uniform sampler2D texture1;
//uniform vec4 texCoords; // Texture coordinates

//void main() {
//    vec2 adjustedTexCoord = vec2(
//        mix(texCoords.x, texCoords.z, TexCoord.x),
//        mix(texCoords.y, texCoords.w, TexCoord.y)
//    );
//    FragColor = texture(texture1, adjustedTexCoord);
//FragColor = vec4(1.0, 0.0, 0.0, 1.0);
//FragColor = vec4(TexCoord, 0.0, 1.0);
//}

in vec2 v_uv;
out vec4 FragColor;

uniform sampler2D u_tex;

void main() {
    FragColor = texture(u_tex, v_uv);
}
