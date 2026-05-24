// Fragment shader for 3D model rendering (placeholder)
// Simple directional lighting

struct FragmentInput {
    float4 position : SV_Position;
    float3 normal   : NORMAL;
    float3 worldPos : TEXCOORD0;
};

cbuffer Uniforms {
    float4x4 model;
    float4x4 view;
    float4x4 projection;
    float3 lightDir;
    float3 lightColor;
    float3 objectColor;
};

[shader("fragment")]
float4 main(FragmentInput input) : SV_Target
{
    float3 norm = normalize(input.normal);
    float3 light = normalize(-lightDir);

    float ambient = 0.15;
    float diffuse = max(dot(norm, light), 0.0);

    float3 color = objectColor * (ambient + diffuse) * lightColor;
    return float4(color, 1.0);
}
