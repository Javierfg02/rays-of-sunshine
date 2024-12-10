#version 330 core

in vec2 uv_coord;

uniform sampler2D colorTexture;
uniform vec2 uvChange;

out vec4 fragColor;

void main()
{
    // fragColor = texture(colorTexture, uv_coord);

    float weights[5] = float[](0.5, 0.25, 0.1, 0.05, 0.025);
    vec2 kernelOffsets[5] = vec2[](
            vec2(-2, 0), vec2(-1, 0), vec2(0,0), vec2(1, 0), vec2(2, 0)
            );

    vec4 kernelAddition = vec4(0.0);
    for (int i = 0; i < 5; i++) {
        vec2 scaledUV = uv_coord + kernelOffsets[i] * uvChange;
        kernelAddition += texture(colorTexture, scaledUV) * weights[i];
    }
    fragColor = kernelAddition * (1.0 / 0.925);
}
