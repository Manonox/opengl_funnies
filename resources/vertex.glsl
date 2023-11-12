#version 330 core
layout (location = 0) in vec3 pos;
layout (location = 1) in vec4 color;

varying vec4 vertex_color;

void main() {
    vertex_color = color;
    gl_Position = vec4(pos.x, pos.y, pos.z, 1.0);
}
