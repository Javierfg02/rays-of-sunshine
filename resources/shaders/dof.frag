#version 330 core

// Task 16: Create a UV coordinate in variable
in vec2 uv_coord;

// Task 8: Add a sampler2D uniform
uniform sampler2D texture;

uniform float kernel[25];
uniform vec2 uvChange;

out vec4 fragColor;

void main()
{
    fragColor = texture(texture, uv_coord);

    fragColor = 1 - texture(texture, uv_coord);
}
