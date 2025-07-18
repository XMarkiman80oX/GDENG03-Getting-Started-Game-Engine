#pragma once
#include "BaseGameObject.h"
#include "VertexBuffer.h"
#include "VertexShader.h"
#include "PixelShader.h"
#include "IndexBuffer.h"
#include "ConstantBuffer.h"

class Sphere : public BaseGameObject
{
public:
    Sphere(std::string name, void* shaderByteCode, size_t sizeShader, RenderSystem* renderSystem);
    ~Sphere();

    void update(RECT windowRect) override;
    void draw(int width, int height) override;
    void initializeObject(void* shaderByteCode, size_t sizeShader) override;

private:
    VertexBufferPtr m_vb;
    IndexBufferPtr m_ib;
    ConstantBufferPtr m_cb;
    VertexShaderPtr m_vs;
    PixelShaderPtr m_ps;

    int m_num_vertices;
    int m_num_indices;
};