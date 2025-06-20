#include "Plane.h"
#include "GraphicsEngine.h"
#include "WorldCamera.h"

Plane::Plane(std::string name, void* shaderByteCode, size_t sizeShader)
    : BaseGameObject(name) {
    this->initializeObject(shaderByteCode, sizeShader);
}

Plane::~Plane() {
    this->vertexBuffer->release();
    this->indexBuffer->release();
    this->constantBuffer->release();
    this->vertexShader->release();
    this->pixelShader->release();
}

void Plane::initializeObject(void* shaderByteCode, size_t sizeShader) {
    vertex planeVertexList[] = {
        {Vector3D(-1.0f, -1.0f, -1.0f), Vector3D(1.0f), Vector3D(0.3f, 0.3f, 0.3f)},
        {Vector3D(-1.0f, -1.0f, 1.0f),  Vector3D(1.0f), Vector3D(0.3f, 0.3f, 0.3f)},
        {Vector3D(1.0f, -1.0f, 1.0f),   Vector3D(1.0f), Vector3D(0.3f, 0.3f, 0.3f)},
        {Vector3D(1.0f, -1.0f, -1.0f),  Vector3D(1.0f), Vector3D(0.3f, 0.3f, 0.3f)},
    };

    this->vertexBuffer = GraphicsEngine::getInstance()->createVertexBuffer();
    UINT plane_size_list = ARRAYSIZE(planeVertexList);

    unsigned int plane_index_list[] = { 0, 1, 2, 2, 3, 0 };

    this->indexBuffer = GraphicsEngine::getInstance()->createIndexBuffer();
    UINT plane_size_index_list = ARRAYSIZE(plane_index_list);
    this->indexBuffer->load(plane_index_list, plane_size_index_list);

    GraphicsEngine::getInstance()->compileVertexShader(L"VertexShader.hlsl", "main", &shaderByteCode, &sizeShader);
    this->vertexShader = GraphicsEngine::getInstance()->createVertexShader(shaderByteCode, sizeShader);

    this->vertexBuffer->load(planeVertexList, sizeof(vertex), plane_size_list, shaderByteCode, sizeShader);
    GraphicsEngine::getInstance()->releaseCompiledShader();

    GraphicsEngine::getInstance()->compilePixelShader(L"PixelShader.hlsl", "main", &shaderByteCode, &sizeShader);
    this->pixelShader = GraphicsEngine::getInstance()->createPixelShader(shaderByteCode, sizeShader);
    GraphicsEngine::getInstance()->releaseCompiledShader();

    constantBufferData cc;
    cc.m_time = 0;

    this->constantBuffer = GraphicsEngine::getInstance()->createConstantBuffer();
    this->constantBuffer->load(&cc, sizeof(constantBufferData));
}

void Plane::update(RECT windowRect) {
}

void Plane::draw(int width, int height) {
    DeviceContext* deviceContextInst = GraphicsEngine::getInstance()->getImmediateDeviceContext();

    deviceContextInst->setVertexShader(this->vertexShader);
    deviceContextInst->setPixelShader(this->pixelShader);
    constantBufferData cbData = {};

    cbData.m_time = ::GetTickCount();

    Matrix4x4 allMatrix;
    allMatrix.setIdentity();

    Matrix4x4 translationMatrix;
    translationMatrix.setTranslation(this->getLocalPosition());

    Matrix4x4 scaleMatrix;
    scaleMatrix.setScale(this->getLocalScale());

    Vector3D rotation = this->getLocalRotation();
    Matrix4x4 zMatrix, yMatrix, xMatrix;
    zMatrix.setIdentity();
    yMatrix.setIdentity();
    xMatrix.setIdentity();

    zMatrix.setRotationZ(rotation.z);
    xMatrix.setRotationX(rotation.x);
    yMatrix.setRotationY(rotation.y);

    Matrix4x4 rotationMatrix;
    rotationMatrix = xMatrix * yMatrix * zMatrix;

    allMatrix *= scaleMatrix;
    allMatrix *= rotationMatrix;
    allMatrix *= translationMatrix;

    cbData.m_world = allMatrix;

    cbData.m_view = WorldCamera::getInstance()->getViewMatrix();
    cbData.m_proj = WorldCamera::getInstance()->getProjectionMatrix();

    this->constantBuffer->update(deviceContextInst, &cbData);
    deviceContextInst->setConstantBuffer(this->vertexShader, this->constantBuffer);
    deviceContextInst->setConstantBuffer(this->pixelShader, this->constantBuffer);

    deviceContextInst->setVertexBuffer(this->vertexBuffer);
    deviceContextInst->setIndexBuffer(this->indexBuffer);

    deviceContextInst->drawIndexedTriangleList(this->indexBuffer->getSizeIndexList(), 0, 0);
}