struct VS_INPUT
{
    float4 position : POSITION; // the " : POSITION" semantic part of the hlsl language 
                                // identifies that this attribute corresponds to "up vertex position"
    float3 color : COLOR;
    float3 color1 : COLOR1;
};
struct VS_OUTPUT //The vertex shader will send the input data to the pixel shader after its execution
{
    float4 position : SV_POSITION; // The " : SV_POSITION " part is a system value semantic
                                   // after the signature of the function. This indicates to the graphics
                                   // pipeline that the output of our vertex shader will contain
                                   // the final transformed vertex position in the screen space coordinates
                                   // used for rasterization
    float3 color : COLOR;
    float3 color1 : COLOR1;
};

//This is the constant buffer that will be passed to this vertex shader
cbuffer constant : register(b0)
{
    row_major float4x4 m_world;
    row_major float4x4 m_view;
    row_major float4x4 m_proj;
    
    unsigned int m_time;
};

VS_OUTPUT main(VS_INPUT input)
{
    VS_OUTPUT output = (VS_OUTPUT) 0; //This creates a VS_OUTPUT object
    //output.position = lerp(input.position /*initial position*/, input.position1/*new position*/, 
    //                        sin(m_time / 1000.0f) + 1.0f / 2.0f /*- delta time
    //                                                              - + 1/2 because delta only accepts [0,1],
    //                                                              by default it's [-1,1] (if only +1, it's going to be
    //                                                              [0,2])*/
    //                        );
    //WORLD SPACE
    output.position = mul(input.position, m_world);
    //VIEW SPACE
    output.position = mul(output.position, m_view);
    //SCREEN SPACE
    output.position = mul(output.position, m_proj);
    
    output.color = input.color;
    output.color1 = input.color1;
    
    return output;
}