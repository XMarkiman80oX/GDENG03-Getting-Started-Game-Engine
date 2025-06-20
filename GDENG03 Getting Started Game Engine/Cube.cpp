#include "Cube.h"
#include "GraphicsEngine.h"
#include "SwapChain.h"
#include "WorldCamera.h"
#include "InputSystem.h"

Cube::Cube(std::string name, void* shaderByteCode, size_t sizeShader) : BaseGameObject(name)
{
	this->initializeObject(shaderByteCode, sizeShader);
}

Cube::~Cube()
{
	this->vertexBuffer->release();
	this->indexBuffer->release();
	this->constantBuffer->release();
	BaseGameObject::~BaseGameObject();
}

void Cube::update(RECT windowRect)
{
	// This function is now only responsible for updating animation variables if needed.
	// The constant buffer update is moved to the draw call for correctness.
	float movementRate = 1.0f / 0.55f;
	this->deltaPos += EngineTime::getDeltaTime() * movementRate;

	if (this->deltaPos > 1.0f)
		deltaPos = 0;
}
void Cube::draw(int width, int height)
{
	DeviceContext* deviceContextInst = GraphicsEngine::getInstance()->getImmediateDeviceContext();

	deviceContextInst->setVertexShader(this->vertexShader);
	deviceContextInst->setPixelShader(this->pixelShader);
	constantBufferData cbData = {};

	// Set the time for animations
	cbData.m_time = ::GetTickCount();

	if (this->deltaPos > 1.0f) {
		this->deltaPos = 0.0f;
	}
	else {
		this->deltaPos += EngineTime::getDeltaTime() * 0.1f;
	}

	Matrix4x4 allMatrix;
	allMatrix.setIdentity();

	Matrix4x4 translationMatrix;
	translationMatrix.setTranslation(this->getLocalPosition());

	Matrix4x4 scaleMatrix;
	scaleMatrix.setIdentity();

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
	deviceContextInst->setConstantBuffer(vertexShader, this->constantBuffer);
	deviceContextInst->setConstantBuffer(pixelShader, this->constantBuffer);

	deviceContextInst->setVertexBuffer(this->vertexBuffer);
	deviceContextInst->setIndexBuffer(this->indexBuffer);

	deviceContextInst->drawIndexedTriangleList(this->indexBuffer->getSizeIndexList(), 0, 0);
}

void Cube::setAnimationSpeed(float speed)
{
	this->speed = speed;
}

void Cube::setRotationSpeed(Vector3D speed)
{
	this->m_rotation_speed = speed;
}

void Cube::initializeObject(void* shaderByteCode, size_t sizeShader)
{
	Vector3D identity = Vector3D(0.0f);

	if(this->getLocalScale() == identity)
		this->setScale(1.0f);
	vertex vertexList[] = {
		//X - Y - Z
		/***************FRONT FACE****************/
		{Vector3D(-0.5f, -0.5f, -0.5f), //POS1
			Vector3D(1,0,0), Vector3D(0.2f,0,0)},
		{Vector3D(-0.5f, 0.5f, -0.5f),    //POS2
			Vector3D(0,1,0), Vector3D(0.2f,0.2f,0)},
		{Vector3D(0.5f, 0.5f, -0.5f),    //POS3
			Vector3D(0,0,1), Vector3D(0.2f,0.2f,0)},
		{Vector3D(0.5f, -0.5f, -0.5f),     //POS4
			Vector3D(1,0,0), Vector3D(0.2f,0,0)},
			/******************************************/

			/***************BACK FACE****************/
			{Vector3D(0.5f, -0.5f, 0.5f), //POS1
				Vector3D(0,1,0), Vector3D(0,0.2f,0)},
			{Vector3D(0.5f, 0.5f, 0.5f),    //POS2
				Vector3D(0,0,1), Vector3D(0,0.2f,0.2f)},
			{Vector3D(-0.5f, 0.5f, 0.5f),    //POS3
				Vector3D(1,0,0), Vector3D(0,0.2f,0.2f)},
			{Vector3D(-0.5f, -0.5f, 0.5f),     //POS4
				Vector3D(1,0,0), Vector3D(0,0.2f,0)},
				/******************************************/
	};

	this->vertexBuffer = GraphicsEngine::getInstance()->createVertexBuffer();
	this->vertexBuffer->load(vertexList, sizeof(vertex), ARRAYSIZE(vertexList), shaderByteCode, sizeShader);

	unsigned int index_list[] = {
		//FRONT SIDE
		0,1,2, // 1st Triangle
		2,3,0, //2nd Triangle
		//BACK SIDE
		4,5,6,
		6,7,4,
		//TOP SIDE
		1,6,5,
		5,2,1,
		//BOTTOM SIDE
		7,0,3,
		3,4,7,
		//RIGHT SIDE
		3,2,5,
		5,4,3,
		//LEFT SIDE
		7,6,1,
		1,0,7
	};
	this->indexBuffer = GraphicsEngine::getInstance()->createIndexBuffer();

	UINT size_index_list = ARRAYSIZE(index_list);
	this->indexBuffer->load(index_list, size_index_list);

	GraphicsEngine::getInstance()->compileVertexShader(L"VertexShader.hlsl", "main", &shaderByteCode, &sizeShader);

	this->vertexShader = GraphicsEngine::getInstance()->createVertexShader(shaderByteCode, sizeShader);

	this->vertexBuffer->load(vertexList, sizeof(vertex), ARRAYSIZE(vertexList), shaderByteCode, sizeShader);

	GraphicsEngine::getInstance()->releaseCompiledShader();

	GraphicsEngine::getInstance()->compilePixelShader(L"PixelShader.hlsl", "main", &shaderByteCode, &sizeShader);

	this->pixelShader = GraphicsEngine::getInstance()->createPixelShader(shaderByteCode, sizeShader);

	GraphicsEngine::getInstance()->releaseCompiledShader();

	constantBufferData cc;
	cc.m_time = 0;

	this->constantBuffer = GraphicsEngine::getInstance()->createConstantBuffer();
	this->constantBuffer->load(&cc, sizeof(constantBufferData));
}

void Cube::onKeyDown(int key)
{
	float moveSpeed = EngineTime::getDeltaTime() * this->speed;

	switch (key) {
		case 'W':
			this->setPosition(this->getLocalPosition() + Vector3D(0.0f, moveSpeed, 0.0f));
			break;

		case 'A':
			this->setPosition(this->getLocalPosition() + Vector3D(-moveSpeed, 0.0f, 0.0f));
			break;

		case 'S':
			this->setPosition(this->getLocalPosition() + Vector3D(0.0f, -moveSpeed, 0.0f));
			break;

		case 'D':
			this->setPosition(this->getLocalPosition() + Vector3D(moveSpeed, 0.0f, 0.0f));
			break;
	}
}

void Cube::onKeyUp(int key)
{
}
