#version 330 core

layout (location = 0) in vec3 pos;
layout (location = 1) in vec2 uv;

out vec2 uv_coord;

void main() {
    gl_Position = vec4(pos, 1.0);
    uv_coord = uv;
}
