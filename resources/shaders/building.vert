#version 330 core

layout (location = 0) in vec3 position;
layout (location = 1) in vec3 normal;
layout (location = 2) in vec3 color;

out vec3 col;
out vec3 worldPos;
out vec3 worldNorm;

uniform mat4 modelMatrix;
uniform mat4 viewMatrix;
uniform mat4 projMatrix;

void main() {
    // transform position to world space
    vec4 worldPosition = modelMatrix * vec4(position, 1.0);
    worldPos = vec3(worldPosition);

    // transform normal to world space
    worldNorm = normalize(mat3(transpose(inverse(modelMatrix))) * normal);

    // transform position to clip space
    gl_Position = projMatrix * viewMatrix * worldPosition;
    col = color;
}
