#version 330 core
layout(location = 0) in vec2 aPos;
layout(location = 1) in vec2 aTexCoord;

uniform mat4 u_model;
uniform mat4 projection;
uniform mat4 view;

uniform vec2 u_uv;
uniform vec2 u_uv_size;

out vec2 v_uv;

void main() {
    gl_Position = projection * view * u_model * vec4(aPos, 0.0, 1.0);
    v_uv = u_uv + aTexCoord * u_uv_size;
}
