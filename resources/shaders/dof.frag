#version 330 core

in vec2 uv_coord;

uniform sampler2D colorTexture;
uniform sampler2D hblurTexture;
uniform sampler2D vblurTexture;
uniform sampler2D depthTexture;

uniform vec2 uvChangeH;
uniform vec2 uvChangeV;

out vec4 fragColor;

float focusDepth = 0.5;
float blurScale = 5.0;

void main()
{
    // fragColor = texture(colorTexture, uv_coord);

    // horizontal
    float weightsH[5] = float[](0.5, 0.25, 0.1, 0.05, 0.025);
    vec2 kernelOffsetsH[5] = vec2[](
            vec2(-2, 0), vec2(-1, 0), vec2(0,0), vec2(1, 0), vec2(2, 0)
            );

    vec4 kernelAdditionH = vec4(0.0);
    for (int i = 0; i < 5; i++) {
        vec2 scaledUV = uv_coord + kernelOffsetsH[i] * uvChangeH;
        kernelAdditionH += texture(hblurTexture, scaledUV) * weightsH[i];
    }
    fragColor = kernelAdditionH * (1.0 / 0.925);

    // vertical
    float weightsV[5] = float[](0.5, 0.25, 0.1, 0.05, 0.025);
    vec2 kernelOffsetsV[5] = vec2[](
            vec2(0, -2), vec2(0, -1), vec2(0,0), vec2(0, 1), vec2(0, 2)
            );

    vec4 kernelAdditionV = vec4(0.0);
    for (int i = 0; i < 5; i++) {
        vec2 scaledUV = uv_coord + kernelOffsetsV[i] * uvChangeV;
        kernelAdditionV += texture(vblurTexture, scaledUV) * weightsV[i];
    }
    fragColor = kernelAdditionV * (1.0 / 0.925);

    // mix
    fragColor = texture(vblurTexture, uv_coord);
    // float depth = texture(depthTexture, uv_coord).r;
    // float blurFactor = clamp(abs(depth - focusDepth) * blurScale, 0.0, 1.0);

    // vec3 originalColor = texture(colorTexture, gl_FragCoord.xy).rgb;
    // vec3 blurredColor = texture(vblurTexture, gl_FragCoord.xy).rgb;

    // // blend
    // vec3 finalColor = mix(originalColor, blurredColor, blurFactor);
    // fragColor = vec4(finalColor, 1.0);

    // fragColor = 1 - texture(colorTexture, uv_coord);
}
