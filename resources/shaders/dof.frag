#version 330 core

in vec2 uv_coord;

uniform sampler2D colorTexture;
uniform sampler2D vblurTexture;
// uniform sampler2D blurredTexture;
uniform sampler2D depthTexture;

// uniform float kernel[25];
// uniform vec2 uvChange;
// uniform float focusDepth;
// uniform float blurScale;

out vec4 fragColor;

float focusDepth = 0.5;
float blurScale = 5.0;

void main()
{
    fragColor = texture(colorTexture, uv_coord);

    // fragColor = texture(vblurTexture, uv_coord);
    // float depth = texture(depthTexture, uv_coord).r;
    // float blurFactor = clamp(abs(depth - focusDepth) * blurScale, 0.0, 1.0);

    // vec3 originalColor = texture(colorTexture, gl_FragCoord.xy).rgb;
    // vec3 blurredColor = texture(vblurTexture, gl_FragCoord.xy).rgb;

    // // blend
    // vec3 finalColor = mix(originalColor, blurredColor, blurFactor);
    // fragColor = vec4(finalColor, 1.0);
}
