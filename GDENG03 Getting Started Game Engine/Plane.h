#pragma once
#include "BaseGameObject.h"
#include "VertexBuffer.h"
#include "IndexBuffer.h"
#include "ConstantBuffer.h"

class Plane : public BaseGameObject
{
public:
    Plane(std::string name, void* shaderByteCode, size_t sizeShader, RenderSystem* renderSystem);
    ~Plane();

    void update(RECT windowRect) override;
    void draw(int width, int height) override;

private:
    VertexBufferPtr m_vertex_buffer;
    IndexBufferPtr m_index_buffer;
    ConstantBufferPtr m_constant_buffer;
    VertexShaderPtr m_vertex_shader;
    PixelShaderPtr m_pixel_shader;

    // Inherited via BaseGameObject
    void initializeObject(void* shaderByteCode, size_t sizeShader) override;
};