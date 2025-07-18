#pragma once
#include "BaseComponent.h"
#include "Prerequisites.h"

class RenderComponent : public BaseComponent
{
public:
    MeshPtr mesh;
    TexturePtr texture;
    VertexShaderPtr vertexShader;
    PixelShaderPtr pixelShader;
    ConstantBufferPtr constantBuffer;
    VertexBufferPtr vertexBuffer;
    IndexBufferPtr indexBuffer;
};