Texture2D Texture : register(t0); // This is the texture that we will sample in the pixel shader)
sampler TextureSampler : register(s0); // This is the sampler that we will use to sample the texture

struct PS_INPUT // We are getting the output data from the vertex shader, so it needs to be
                // identical to the VS_OUTPUT structure from the vertex shader hlsl file
{
    float4 position : SV_POSITION;
    float2 texcoord : TEXTCOORD0;
};

//This is the constant buffer that will be passed to this pixel shader
cbuffer constant : register(b0)
{
    row_major float4x4 m_world;
    row_major float4x4 m_view;
    row_major float4x4 m_proj;
    
    unsigned int m_time;
};

/*
    SV_TARGET is a semantic name. A semnatic name is "a string attached to a shader input or output
that conveys information about the intended use of a parameter". In this case SV_Target stands
for "system value target" and tells us that the outputs of our pixel shader will be stored in the first 
render target by native graphics pipeline, specifically in the output merger stage (the last stage after pixel
shader stage). We have to bind the render target function by calling the setRenderTargets function.
*/
float4 main(PS_INPUT input) : SV_TARGET
{
    return Texture.Sample(TextureSampler, input.texcoord);
}