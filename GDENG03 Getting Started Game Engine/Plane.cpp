#include "Plane.h"
#include "GraphicsEngine.h"
#include "DeviceContext.h"
#include "ConstantBuffer.h"

Plane::Plane(std::string name) : BaseGameObject(name)
{
    //Set the vertices of the plane here
    vertex planeVertexList[] = {
        //X - Y - Z
        {Vector3D(-1.0f, -1.0f, -1.0f), Vector3D(0.3f,0.3f,0.3f), Vector3D(0.3f,0.3f,0.3f)},
        {Vector3D(-1.0f, -1.0f,  1.0f), Vector3D(0.3f,0.3f,0.3f), Vector3D(0.3f,0.3f,0.3f)},
        {Vector3D(1.0f, -1.0f,  1.0f), Vector3D(0.3f,0.3f,0.3f), Vector3D(0.3f,0.3f,0.3f)},
        {Vector3D(1.0f, -1.0f, -1.0f), Vector3D(0.3f,0.3f,0.3f), Vector3D(0.3f,0.3f,0.3f)},
    };

    m_vertex_buffer = GraphicsEngine::getInstance()->createVertexBuffer();
    UINT plane_size_list = ARRAYSIZE(planeVertexList);

    unsigned int plane_index_list[] = {
        0,1,2,
        2,3,0
    };

    m_index_buffer = GraphicsEngine::getInstance()->createIndexBuffer();
    UINT plane_size_index_list = ARRAYSIZE(plane_index_list);
    m_index_buffer->load(plane_index_list, plane_size_index_list);

    void* shader_byte_code = nullptr;
    size_t size_shader = 0;
    GraphicsEngine::getInstance()->compileVertexShader(L"VertexShader.hlsl", "main", &shader_byte_code, &size_shader);
    m_vertex_buffer->load(planeVertexList, sizeof(vertex), plane_size_list, shader_byte_code, size_shader);
    GraphicsEngine::getInstance()->releaseCompiledShader();

    //Initialize transform
    this->setPosition(0.0f, 0.0f, 0.0f);
    this->setScale(1.0f, 1.0f, 1.0f);
    this->setRotation(0.0f, 0.0f, 0.0f);
}

Plane::~Plane()
{
    if (m_vertex_buffer) m_vertex_buffer->release();
    if (m_index_buffer) m_index_buffer->release();
}

void Plane::update(float deltaTime)
{
    //The plane is static for now, so no updates are needed.
}

void Plane::draw(int width, int height, VertexShader* vertexShader, PixelShader* pixelShader)
{
    // The constant buffer is expected to be set by the main render loop in AppWindow
    // before this draw call.
    GraphicsEngine::getInstance()->getImmediateDeviceContext()->setVertexBuffer(m_vertex_buffer);
    GraphicsEngine::getInstance()->getImmediateDeviceContext()->setIndexBuffer(m_index_buffer);
    GraphicsEngine::getInstance()->getImmediateDeviceContext()->drawIndexedTriangleList(m_index_buffer->getSizeIndexList(), 0, 0);
}