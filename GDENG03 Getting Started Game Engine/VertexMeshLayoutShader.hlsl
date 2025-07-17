struct VS_INPUT
{
    float4 position : POSITION; 
    float2 texcoord : TEXCOORD0;
};
struct VS_OUTPUT //The vertex shader will send the input data to the pixel shader after its execution
{
    float4 position : SV_POSITION; 
    float2 texcoord : TEXCOORD0;
};

VS_OUTPUT main(VS_INPUT input)
{
    VS_OUTPUT output = (VS_OUTPUT) 0; 

    return output;
}