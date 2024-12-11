#version 330 core

out vec4 fragColor;

in vec3 worldPos;
in vec3 worldNorm;
in vec3 col;

// Light struct
struct Light {
    int type;  // 0: point, 1: directional, 2: spot
    vec3 color;
    vec3 function; // Attenuation coefficients (constant, linear, quadratic)
    vec4 pos;      // Position for point/spot lights
    vec4 dir;      // Direction for directional/spot lights
    float penumbra; // For spot lights only
    float angle;    // For spot lights only
};

// Material struct
struct Material {
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
    float shininess;
};

// Uniforms
uniform int numLights; // should only be one light actually, but will keep it as a vector
uniform Light lights[1];
uniform Material material;
uniform vec3 cameraPos;

// Global coefficients
uniform float ka; // Ambient coefficient
uniform float kd; // Diffuse coefficient
uniform float ks; // Specular coefficient

// Helper functions
// calculate attenuation of a light
float getLightAttenuation(Light light, float distance) {
    return 1.0 / (light.function.x + light.function.y * distance + light.function.z * distance * distance);
}

vec3 calculateLight(Light light, vec3 normal, vec3 cameraDir) {
    // calculate light direction
    vec3 lightDir = vec3(0.0);
    float attenuation = 1.0;
    float falloff = 1.0;
    bool isWindow = (length(col) > 1.7);  // lit windows will have brighter colors


    if (light.type == 0) { // point light
        lightDir = normalize(light.pos.xyz - worldPos);
        float distance = length(light.pos.xyz - worldPos);
        attenuation = getLightAttenuation(light, distance);
    }
    else if (light.type == 1) { // directional light
        lightDir = normalize(-light.dir.xyz);
    }
    else { // spot light
        vec3 lightToPoint = normalize(light.pos.xyz - worldPos);
        lightDir = normalize(-light.dir.xyz);
        float distance = length(light.pos.xyz - worldPos);
        attenuation = getLightAttenuation(light, distance);

        // output illumination
        float theta = acos(dot(lightToPoint, lightDir));
        float thetaOuter = light.angle;
        float thetaInner = thetaOuter - light.penumbra;

        if (theta > thetaOuter) {
            if (isWindow) {
                // keep lit windows visible even in darkness
                return col * 0.15;
            }
            return vec3(0.0);
        }

        if (theta > thetaInner) {
            float x = (theta - thetaInner) / (thetaOuter - thetaInner);
            falloff = -2.0 * pow(x, 3) + 3.0 * pow(x, 2);
            falloff = 1.0 - falloff;
            attenuation *= falloff;
        }
    }

    // calculate reflection direction
    vec3 reflectDir = reflect(-lightDir, normal);

    // ambient
    vec3 ambient = ka * material.ambient * light.color;

    // diffuse
    float diff = max(dot(normal, lightDir), 0.0);
    vec3 diffuse = kd * diff * material.diffuse * light.color;

    // specular
    float spec = pow(max(dot(cameraDir, reflectDir), 0.0), max(material.shininess, 0.01));
    vec3 specular = ks * spec * material.specular * light.color;

    vec3 result;
    if (isWindow) {
        ambient *= 1.5; // brighter ambient
        specular *= 2.0; // stronger specular

        // add emissive component for lit windows
        vec3 emissive = col * 0.3;  // base glow from the window

        result = ambient + attenuation * (diffuse + specular) + emissive;
        return max(result, col * 0.15);  // ensure windows never go completely dark
    } else {
        // regular lighting for walls
        result = ambient + attenuation * (diffuse + specular);
        return result;
    }

    return ambient + attenuation * (diffuse + specular);
}

void main() {

    // normalize
    vec3 normal = normalize(worldNorm);

    // camera, light, reflect direction
    vec3 cameraDir = normalize(cameraPos - worldPos);

    // final illumination color
    vec3 illumination = vec3(0.0);

    // calculate the contribution from each light
    for(int i = 0; i < numLights; i++) {
        illumination += calculateLight(lights[i], normal, cameraDir);
    }

    fragColor = vec4(illumination, 1.0);
}
