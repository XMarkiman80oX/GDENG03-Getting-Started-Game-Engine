#pragma once
#include "BaseGameObject.h"
#include "ConstantBuffer.h"
#include "DeviceContext.h"
#include "IndexBuffer.h"
#include "PixelShader.h"
#include "VertexShader.h"
#include "VertexBuffer.h"

class Plane : public BaseGameObject {
public:
    Plane(std::string name, void* shaderByteCode, size_t sizeShader);
    ~Plane();

    void update(RECT windowRect) override;
    void draw(int width, int height) override;
    void initializeObject(void* shaderByteCode, size_t sizeShader) override;

private:
    VertexBuffer* vertexBuffer;
    VertexShader* vertexShader;
    PixelShader* pixelShader;
    IndexBuffer* indexBuffer;
    ConstantBuffer* constantBuffer;
};