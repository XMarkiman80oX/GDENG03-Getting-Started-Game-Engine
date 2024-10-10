struct PS_INPUT // We are getting the output data from the vertex shader, so it needs to be
                // identical to the VS_OUTPUT structure from the vertex shader hlsl file
{
    float4 position : SV_POSITION;
    float3 color : COLOR;
    float3 color1 : COLOR1;
};

//This is the constant buffer that will be passed to this pixel shader
cbuffer constant : register(b0)
{
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
    return float4(lerp(input.color, input.color1, sin(m_time / 500.0f) + 1.0f / 2.0f), 1.0f);
}