#version 330 core
layout(location = 0) in vec4 vertex; // <vec3 pos, vec2 tex>
out vec2 TexCoords;

uniform mat4 projection;
uniform mat4 view;
uniform vec3 worldPosition; // The world-space position of the text

void main() {
    vec3 finalPos = worldPosition + vec3(vertex.xy, 0.0); // Convert vec2 to vec3
    vec4 worldPos = vec4(finalPos, 1.0);
    gl_Position = projection * view * worldPos;
    TexCoords = vertex.zw;
}

