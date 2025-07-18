#include "Plane.h"
#include "GraphicsEngine.h"
#include "DeviceContext.h"
#include "ConstantBuffer.h"
#include "WorldCamera.h"

Plane::Plane(std::string name, void* shaderByteCode, size_t sizeShader, RenderSystem* renderSystem) : BaseGameObject(name, renderSystem)
{
    this->initializeObject(shaderByteCode, sizeShader);
}

Plane::~Plane()
{
}

void Plane::update(RECT windowRect)
{
    //The plane is static for now, so no updates are needed.
}

void Plane::draw(int width, int height)
{

    DeviceContextPtr deviceContext = GraphicsEngine::getInstance()->getRenderSystem()->getImmediateDeviceContext();

    deviceContext->setVertexShader(this->m_vertex_shader);
    deviceContext->setPixelShader(this->m_pixel_shader);
    deviceContext->setTexture(this->m_pixel_shader, this->texture);

    constantBufferData cbd = {};
    cbd.m_time = 0;

    Matrix4x4 world;
    world.setIdentity();

    Matrix4x4 translation;
    translation.setTranslation(this->getLocalPosition());

    Matrix4x4 scale;
    scale.setScale(this->getLocalScale());

    world *= scale;
    world *= translation;

    cbd.m_world = world;
    cbd.m_view = WorldCamera::getInstance()->getViewMatrix();
    cbd.m_proj = WorldCamera::getInstance()->getProjectionMatrix();

    this->m_constant_buffer->update(deviceContext, &cbd);

    deviceContext->setConstantBuffer(this->m_vertex_shader, this->m_constant_buffer);
    deviceContext->setConstantBuffer(this->m_pixel_shader, this->m_constant_buffer);

    deviceContext->setVertexBuffer(m_vertex_buffer);
    deviceContext->setIndexBuffer(m_index_buffer);
    deviceContext->drawIndexedTriangleList(m_index_buffer->getSizeIndexList(), 0, 0);
}

void Plane::initializeObject(void* shaderByteCode, size_t sizeShader)
{
    //white plane
    unsigned char white_pixel[] = { 255, 255, 255, 255 };
    this->texture = GraphicsEngine::getInstance()->getTextureManager()->createTextureFromPixelData(white_pixel, 1, 1);

    //Set the vertices of the plane here
    vertex planeVertexList[] = {
        //X - Y - Z
        {Vector3D(-1.0f, -1.0f, -1.0f), Vector2D(0.0f, 1.0f)},
        {Vector3D(-1.0f, -1.0f,  1.0f), Vector2D(0.0f, 0.0f)},
        {Vector3D(1.0f, -1.0f,  1.0f), Vector2D(1.0f, 0.0f)},
        {Vector3D(1.0f, -1.0f, -1.0f), Vector2D(1.0f, 1.0f)},
    };

    UINT plane_size_list = ARRAYSIZE(planeVertexList);

    unsigned int plane_index_list[] = {
        0,1,2,
        2,3,0
    };

    UINT plane_size_index_list = ARRAYSIZE(plane_index_list);

    GraphicsEngine::getInstance()->getRenderSystem()->compileVertexShader(L"VertexShader.hlsl", "main", &shaderByteCode, &sizeShader);
    this->m_vertex_shader = GraphicsEngine::getInstance()->getRenderSystem()->createVertexShader(shaderByteCode, sizeShader);
    this->m_vertex_buffer = GraphicsEngine::getInstance()->getRenderSystem()->createVertexBuffer(planeVertexList, sizeof(vertex), plane_size_list, shaderByteCode, sizeShader);
    GraphicsEngine::getInstance()->getRenderSystem()->releaseCompiledShader();

    this->m_index_buffer = GraphicsEngine::getInstance()->getRenderSystem()->createIndexBuffer(plane_index_list, plane_size_index_list);

    GraphicsEngine::getInstance()->getRenderSystem()->compilePixelShader(L"PixelShader.hlsl", "main", &shaderByteCode, &sizeShader);
    this->m_pixel_shader = GraphicsEngine::getInstance()->getRenderSystem()->createPixelShader(shaderByteCode, sizeShader);
    GraphicsEngine::getInstance()->getRenderSystem()->releaseCompiledShader();

    constantBufferData cbd = {};
    cbd.m_time = 0;

    this->m_constant_buffer = GraphicsEngine::getInstance()->getRenderSystem()->createConstantBuffer(&cbd, sizeof(constantBufferData));

    //Initialize transform
    this->setPosition(0.0f, 0.0f, 0.0f);
    this->setScale(1.0f, 1.0f, 1.0f);
    this->setRotation(0.0f, 0.0f, 0.0f);
}