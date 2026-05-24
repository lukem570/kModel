// Vertex shader for 3D model rendering (placeholder)
// Will be compiled with slangc when Vulkan rendering is enabled

struct VertexInput {
    float3 position : POSITION;
    float3 normal   : NORMAL;
};

struct VertexOutput {
    float4 position : SV_Position;
    float3 normal   : NORMAL;
    float3 worldPos : TEXCOORD0;
};

cbuffer Uniforms {
    float4x4 model;
    float4x4 view;
    float4x4 projection;
};

[shader("vertex")]
VertexOutput main(VertexInput input)
{
    VertexOutput output;
    float4 worldPos = mul(model, float4(input.position, 1.0));
    output.worldPos = worldPos.xyz;
    output.position = mul(projection, mul(view, worldPos));
    output.normal = mul((float3x3)model, input.normal);
    return output;
}
