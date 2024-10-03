struct PS_INPUT
{
    float4 position : SV_POSITION; // Input position from vertex shader
    float2 texCoord : TEXCOORD; // Input texture coordinates
    float3 color1 : COLOR; // First color
    float3 color2 : COLOR1; // Second color
};

float4 psmain(PS_INPUT input) : SV_Target
{
    // Use input.color1, input.color2, or texture sampling as needed
    return float4(input.color1 * 0.5 + input.color2 * 0.5, 1.0); // Example blending
}