struct VS_INPUT
{
    float4 position1 : POSITION; // First position
    float4 position2 : POSITION; // Second position
    float3 color1 : COLOR; // First color
    float3 color2 : COLOR1; // Second color
    float2 texCoord : TEXCOORD; // Texture coordinates
};

struct PS_INPUT
{
    float4 position : SV_POSITION; // Output position for pixel shader
    float2 texCoord : TEXCOORD; // Pass through texture coordinates
    float3 color1 : COLOR; // Pass through first color
    float3 color2 : COLOR1; // Pass through second color
};

PS_INPUT vsmain(VS_INPUT input)
{
    PS_INPUT output;
    output.position = input.position1; // Or use some transformation if needed
    output.texCoord = input.texCoord; // Pass through texture coordinates
    output.color1 = input.color1; // Pass through first color
    output.color2 = input.color2; // Pass through second color
    return output;
}