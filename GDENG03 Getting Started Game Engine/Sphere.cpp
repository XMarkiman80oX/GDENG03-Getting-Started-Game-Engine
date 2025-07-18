#include "Sphere.h"
#include "GraphicsEngine.h"
#include "DeviceContext.h"
#include "WorldCamera.h"
#include <vector>

#define M_PI 3.14159265358979323846

Sphere::Sphere(std::string name, void* shaderByteCode, size_t sizeShader, RenderSystem* renderSystem) : BaseGameObject(name, renderSystem)
{
    this->initializeObject(shaderByteCode, sizeShader);
}

Sphere::~Sphere()
{
}

void Sphere::initializeObject(void* shaderByteCode, size_t sizeShader)
{
    // Sphere generation
    const int stacks = 20;
    const int slices = 20;
    const float radius = 1.0f;

    std::vector<vertex> vertices;
    std::vector<unsigned int> indices;

    // Generate vertices
    for (int i = 0; i <= stacks; ++i)
    {
        float V = (float)i / (float)stacks;
        float phi = V * M_PI;

        for (int j = 0; j <= slices; ++j)
        {
            float U = (float)j / (float)slices;
            float theta = U * (M_PI * 2);

            float x = radius * cos(theta) * sin(phi);
            float y = radius * cos(phi);
            float z = radius * sin(theta) * sin(phi);

            vertices.push_back({ Vector3D(x, y, z), Vector2D(U, V) });
        }
    }

    m_num_vertices = vertices.size();

    // Generate indices
    for (int i = 0; i < slices * stacks + slices; ++i)
    {
        indices.push_back(i);
        indices.push_back(i + slices + 1);
        indices.push_back(i + slices);

        indices.push_back(i + slices + 1);
        indices.push_back(i);
        indices.push_back(i + 1);
    }
    m_num_indices = indices.size();


    // Create Buffers
    GraphicsEngine::getInstance()->getRenderSystem()->compileVertexShader(L"VertexShader.hlsl", "main", &shaderByteCode, &sizeShader);
    m_vs = GraphicsEngine::getInstance()->getRenderSystem()->createVertexShader(shaderByteCode, sizeShader);
    m_vb = GraphicsEngine::getInstance()->getRenderSystem()->createVertexBuffer(vertices.data(), sizeof(vertex), m_num_vertices, shaderByteCode, sizeShader);
    GraphicsEngine::getInstance()->getRenderSystem()->releaseCompiledShader();

    m_ib = GraphicsEngine::getInstance()->getRenderSystem()->createIndexBuffer(indices.data(), m_num_indices);

    // Shaders
    GraphicsEngine::getInstance()->getRenderSystem()->compilePixelShader(L"PixelShader.hlsl", "main", &shaderByteCode, &sizeShader);
    m_ps = GraphicsEngine::getInstance()->getRenderSystem()->createPixelShader(shaderByteCode, sizeShader);
    GraphicsEngine::getInstance()->getRenderSystem()->releaseCompiledShader();

    // Constant Buffer
    constantBufferData cbd = {};
    cbd.m_time = 0;
    m_cb = GraphicsEngine::getInstance()->getRenderSystem()->createConstantBuffer(&cbd, sizeof(constantBufferData));
}


void Sphere::update(RECT windowRect)
{
    // No updates for now
}

void Sphere::draw(int width, int height)
{
    DeviceContextPtr deviceContext = GraphicsEngine::getInstance()->getRenderSystem()->getImmediateDeviceContext();

    deviceContext->setVertexShader(m_vs);
    deviceContext->setPixelShader(m_ps);

    constantBufferData cbd = {};
    cbd.m_time = EngineTime::EngineTime::getTotalElapsedTime() * 1000.0;

    Matrix4x4 world;
    world.setIdentity();

    Matrix4x4 translation;
    translation.setTranslation(getLocalPosition());

    Matrix4x4 scale;
    scale.setScale(getLocalScale());

    world *= scale;
    world *= translation;

    cbd.m_world = world;
    cbd.m_view = WorldCamera::getInstance()->getViewMatrix();
    cbd.m_proj = WorldCamera::getInstance()->getProjectionMatrix();

    m_cb->update(deviceContext, &cbd);

    deviceContext->setConstantBuffer(m_vs, m_cb);
    deviceContext->setConstantBuffer(m_ps, m_cb);

    deviceContext->setVertexBuffer(m_vb);
    deviceContext->setIndexBuffer(m_ib);

    deviceContext->drawIndexedTriangleList(m_num_indices, 0, 0);
}