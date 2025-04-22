#version 330 core
layout(location = 0) in vec2 aPos;
uniform mat4 u_Proj;
uniform mat4 u_View;
void main() {
    gl_Position = u_Proj * u_View * vec4(aPos, 0, 1);
}
