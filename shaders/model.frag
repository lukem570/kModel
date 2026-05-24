#version 440

layout(location = 0) in vec3 vNormal;
layout(location = 1) in vec3 vWorldPos;

layout(location = 0) out vec4 fragColor;

layout(std140, binding = 0) uniform buf {
    mat4 mvp;
    mat4 modelMatrix;
    vec4 eyePos;
    vec4 objectColor;
};

// Approximate environment map: gradient sky dome
vec3 sampleEnv(vec3 dir)
{
    float t = 0.5 + 0.5 * dir.y;
    vec3 horizon = vec3(0.25, 0.27, 0.35);
    vec3 zenith  = vec3(0.08, 0.12, 0.24);
    vec3 ground  = vec3(0.10, 0.08, 0.06);
    vec3 sky = mix(horizon, zenith, clamp(t * 2.0 - 0.5, 0.0, 1.0));
    return mix(ground, sky, smoothstep(-0.05, 0.1, dir.y));
}

void main()
{
    vec3 N = normalize(vNormal);
    vec3 V = normalize(eyePos.xyz - vWorldPos);

    // Flip normal for back-facing triangles (STL winding may be inconsistent)
    if (dot(N, V) < 0.0)
        N = -N;

    // Material params
    float metallic = 0.25;
    float roughness = 0.45;
    vec3 baseColor = objectColor.rgb;

    // Fresnel (Schlick)
    float NdotV = max(dot(N, V), 0.0);
    vec3 F0 = mix(vec3(0.04), baseColor, metallic);
    vec3 fresnel = F0 + (1.0 - F0) * pow(1.0 - NdotV, 5.0);

    // --- 3-point lighting ---

    // Key light: warm, from upper-right-front
    vec3 keyDir = normalize(vec3(0.6, 0.8, 0.4));
    vec3 keyColor = vec3(1.0, 0.97, 0.92) * 1.2;
    float keyNdotL = max(dot(N, keyDir), 0.0);
    vec3 keyHalf = normalize(keyDir + V);
    float keyNdotH = max(dot(N, keyHalf), 0.0);
    float specPower = 2.0 / (roughness * roughness + 0.001) - 2.0;
    float keySpec = pow(keyNdotH, specPower);

    // Fill light: cool, from lower-left
    vec3 fillDir = normalize(vec3(-0.5, 0.2, -0.4));
    vec3 fillColor = vec3(0.35, 0.45, 0.65);
    float fillNdotL = max(dot(N, fillDir), 0.0) * 0.6;

    // Back light: edge highlight from behind
    vec3 backDir = normalize(vec3(-0.2, 0.3, -0.8));
    vec3 backColor = vec3(0.6, 0.65, 0.8);
    float backNdotL = max(dot(N, backDir), 0.0) * 0.3;

    // Diffuse (non-metallic contribution)
    vec3 diffuseColor = baseColor * (1.0 - metallic);
    vec3 diffuse = diffuseColor * (
        keyNdotL * keyColor +
        fillNdotL * fillColor +
        backNdotL * backColor
    );

    // Specular from lights
    vec3 specular = fresnel * keySpec * keyColor * keyNdotL * 0.5;

    // Environment reflection
    vec3 R = reflect(-V, N);
    vec3 envColor = sampleEnv(R);
    // Rough surfaces see blurred (dimmer) reflections
    float envStrength = mix(0.8, 0.15, roughness);
    vec3 envReflection = envColor * fresnel * envStrength;

    // Ambient: hemisphere blend (sky above, ground below)
    vec3 skyAmb = vec3(0.12, 0.15, 0.22);
    vec3 gndAmb = vec3(0.06, 0.05, 0.04);
    float hemi = 0.5 + 0.5 * N.y;
    vec3 ambient = diffuseColor * mix(gndAmb, skyAmb, hemi);

    // Fresnel rim glow
    float rim = pow(1.0 - NdotV, 4.0) * 0.3;
    vec3 rimContrib = rim * vec3(0.5, 0.6, 0.75);

    // Combine
    vec3 color = diffuse + specular + envReflection + ambient + rimContrib;

    // ACES tone mapping
    color = (color * (2.51 * color + 0.03)) / (color * (2.43 * color + 0.59) + 0.14);

    // Gamma
    color = pow(color, vec3(1.0 / 2.2));

    fragColor = vec4(color, 1.0);
}
