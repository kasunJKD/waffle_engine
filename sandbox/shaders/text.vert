#version 330 core
layout(location = 0) in vec4 vertex; // <vec3 pos, vec2 tex>
out vec2 TexCoords;

uniform mat4 projection;
uniform mat4 view;
uniform vec3 worldPosition; // The world-space position of the text

void main() {
    vec3 finalPos = vec3(vertex.xy + worldPosition.xy, worldPosition.z); // now uses z
    gl_Position = projection * view * vec4(finalPos, 1.0);
    TexCoords = vertex.zw;
}
