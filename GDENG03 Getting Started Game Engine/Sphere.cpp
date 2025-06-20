#include "Sphere.h"
#include "GraphicsEngine.h"
#include "DeviceContext.h"
#include "WorldCamera.h"
#include <vector>

#define M_PI 3.14159265358979323846

Sphere::Sphere(std::string name, void* shaderByteCode, size_t sizeShader) : BaseGameObject(name)
{
    initializeObject(shaderByteCode, sizeShader);
}

Sphere::~Sphere()
{
    m_vb->release();
    m_ib->release();
    m_cb->release();
    m_vs->release();
    m_ps->release();
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

            // Determine color based on the x-coordinate
            Vector3D color;
            if (x < 0) {
                color = Vector3D(1.0f, 0.0f, 0.0f); // Red
            }
            else {
                color = Vector3D(0.0f, 0.0f, 1.0f); // Blue
            }
            // The second color is for interpolation purposes
            vertices.push_back({ Vector3D(x, y, z), color, color });
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
    m_vb = GraphicsEngine::getInstance()->createVertexBuffer();
    m_vb->load(vertices.data(), sizeof(vertex), m_num_vertices, shaderByteCode, sizeShader);

    m_ib = GraphicsEngine::getInstance()->createIndexBuffer();
    m_ib->load(indices.data(), m_num_indices);

    // Shaders
    GraphicsEngine::getInstance()->compileVertexShader(L"VertexShader.hlsl", "main", &shaderByteCode, &sizeShader);
    m_vs = GraphicsEngine::getInstance()->createVertexShader(shaderByteCode, sizeShader);
    m_vb->load(vertices.data(), sizeof(vertex), m_num_vertices, shaderByteCode, sizeShader);
    GraphicsEngine::getInstance()->releaseCompiledShader();

    GraphicsEngine::getInstance()->compilePixelShader(L"PixelShader.hlsl", "main", &shaderByteCode, &sizeShader);
    m_ps = GraphicsEngine::getInstance()->createPixelShader(shaderByteCode, sizeShader);
    GraphicsEngine::getInstance()->releaseCompiledShader();

    // Constant Buffer
    constantBufferData cbd = {};
    cbd.m_time = 0;
    m_cb = GraphicsEngine::getInstance()->createConstantBuffer();
    m_cb->load(&cbd, sizeof(constantBufferData));
}


void Sphere::update(RECT windowRect)
{
    // No updates for now
}

void Sphere::draw(int width, int height)
{
    DeviceContext* deviceContext = GraphicsEngine::getInstance()->getImmediateDeviceContext();

    deviceContext->setVertexShader(m_vs);
    deviceContext->setPixelShader(m_ps);

    constantBufferData cbd = {};
    cbd.m_time = ::GetTickCount();

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