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
    Sphere(std::string name, void* shaderByteCode, size_t sizeShader);
    ~Sphere();

    void update(RECT windowRect) override;
    void draw(int width, int height) override;
    void initializeObject(void* shaderByteCode, size_t sizeShader) override;

private:
    VertexBuffer* m_vb;
    IndexBuffer* m_ib;
    ConstantBuffer* m_cb;
    VertexShader* m_vs;
    PixelShader* m_ps;

    int m_num_vertices;
    int m_num_indices;
};