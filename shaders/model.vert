#version 440

layout(location = 0) in vec3 position;
layout(location = 1) in vec3 normal;

layout(location = 0) out vec3 vNormal;
layout(location = 1) out vec3 vWorldPos;

layout(std140, binding = 0) uniform buf {
    mat4 mvp;
    mat4 modelMatrix;
    vec4 eyePos;
    vec4 objectColor;
};

void main()
{
    vec4 worldPos = modelMatrix * vec4(position, 1.0);
    vWorldPos = worldPos.xyz;
    vNormal = mat3(modelMatrix) * normal;
    gl_Position = mvp * vec4(position, 1.0);
}
