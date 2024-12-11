#version 330 core

in vec2 uv_coord;
out vec4 fragColor;

uniform sampler2D colorTexture;
uniform sampler2D hblurTexture;
uniform sampler2D vblurTexture;
uniform sampler2D depthTexture;

uniform vec2 uvChangeH;
uniform vec2 uvChangeV;
const float focusDepth = 0.5;
const float blurScale = 5.0;

void main() {
    // float weights[5] = float[](0.2, 0.15, 0.1, 0.05, 0.025);

    // // horizontal blur
    // vec2 kernelOffsetsH[5] = vec2[](
    //     vec2(-2, 0), vec2(-1, 0), vec2(0, 0), vec2(1, 0), vec2(2, 0)
    // );
    // vec4 kernelAdditionH = vec4(0.0);
    // for (int i = 0; i < 5; i++) {
    //     vec2 scaledUV = gl_FragCoord.xy / textureSize(colorTexture, 0) + kernelOffsetsH[i] * uvChangeH;
    //     kernelAdditionH += texture(hblurTexture, scaledUV) * weights[i];
    // }

    // // vertical blur
    // vec2 kernelOffsetsV[5] = vec2[](
    //     vec2(0, -2), vec2(0, -1), vec2(0, 0), vec2(0, 1), vec2(0, 2)
    // );
    // vec4 kernelAdditionV = vec4(0.0);
    // for (int i = 0; i < 5; i++) {
    //     vec2 scaledUV = gl_FragCoord.xy / textureSize(colorTexture, 0) + kernelOffsetsV[i] * uvChangeV;
    //     kernelAdditionV += texture(vblurTexture, scaledUV) * weights[i];
    // }

    // // combine horizontal and vertical blurred results
    // vec4 blurredColor = (kernelAdditionH + kernelAdditionV) * (1.0 / 0.925);

    // // get original color and depth
    // vec3 originalColor = texture(colorTexture, gl_FragCoord.xy / textureSize(colorTexture, 0)).rgb;
    // float depth = texture(depthTexture, gl_FragCoord.xy / textureSize(depthTexture, 0)).r;

    // // calculate blur factor based on depth
    // float blurFactor = clamp(abs(depth - focusDepth) * blurScale, 0.0, 1.0);

    // // blend original and blurred color
    // vec3 finalColor = mix(originalColor, blurredColor.rgb, blurFactor);

    // fragColor = vec4(finalColor, 1.0);

    fragColor = texture(colorTexture, uv_coord);

}
