#pragma once
#include "BaseGameObject.h"
#include "VertexBuffer.h"
#include "IndexBuffer.h"

class Plane : public BaseGameObject
{
public:
    Plane(std::string name);
    ~Plane();

    void update(float deltaTime) override;
    void draw(int width, int height, VertexShader* vertexShader, PixelShader* pixelShader) override;

private:
    VertexBuffer* m_vertex_buffer;
    IndexBuffer* m_index_buffer;
};