#pragma once
#include "BaseGameObject.h"
#include "VertexBuffer.h"
#include "IndexBuffer.h"

class Plane : public BaseGameObject
{
public:
    Plane(std::string name);
    ~Plane();

    void update(float deltaTime, RECT windowRect) override;
    void draw(int width, int height) override;

private:
    VertexBuffer* m_vertex_buffer;
    IndexBuffer* m_index_buffer;

    // Inherited via BaseGameObject
    void initializeObject(void* shaderByteCode, size_t sizeShader) override;
};