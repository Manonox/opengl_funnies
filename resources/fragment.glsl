#version 330 core
out vec4 FragColor;

uniform float time;

varying vec4 vertex_color;

void main() {
    FragColor = vec4(vertex_color.rgb + vec3(sin(time * 3.127), cos(time * 1.123), -1.0) * 0.5 + 0.5, 1.0);
}
