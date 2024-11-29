#version 330 core

out vec4 fragColor;
in vec3 col;

void main() {

    fragColor = vec4(col, 1.0);

}
