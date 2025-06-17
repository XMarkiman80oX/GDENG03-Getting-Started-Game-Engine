#include "Cube.h"
#include "GraphicsEngine.h"
#include "SwapChain.h"
#include "WorldCamera.h"

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

void Cube::update(float deltaTime, RECT windowRect)
{
	WorldCamera* worldCamera = WorldCamera::getInstance();

	constantBufferData cc;
	cc.m_time = ::GetTickCount();/*This is a windows function that allows us to get the time elapsed since the
								system started in milliseconds*/

	float movementRate = 1.0f / 0.55f;
	//This means we reach one unit per 1/movementRate seconds (reciprocal)
	this->deltaPos += deltaTime * movementRate;

	if (this->deltaPos > 1.0f)
		deltaPos = 0;

	Matrix4x4 temp;
	this->deltaPos += deltaTime * movementRate;

	cc.m_world.setIdentity();
	float cubeSizeMultiplier = 1 / 300.0f;

	worldCamera->setRotationX(rotationX);
	worldCamera->setRotationY(rotationY);

	//moving through the z axis
	//float value entails how much units is moved
	worldCamera->moveThroughX(this->forward);
	worldCamera->moveThroughZ(this->rightward);

	cc.m_view = worldCamera->getInverseBufferCamera();
	cc.m_proj.setOrthogonalProjectionMatrix(
		(windowRect.right - windowRect.left) * cubeSizeMultiplier,
		(windowRect.bottom - windowRect.top) * cubeSizeMultiplier,
		-4.0f,
		4.0f
	);
	int width = (windowRect.right - windowRect.left);
	int height = (windowRect.bottom - windowRect.top);

	cc.m_proj.setPerspectiveFOVLH(1.57f, (float)width / (float)height, 0.1f, 100.0f);
	this->constantBuffer->update(GraphicsEngine::getInstance()->getImmediateDeviceContext(), &cc);
}

void Cube::draw(int width, int height)
{
	DeviceContext* deviceContextInst = GraphicsEngine::getInstance()->getImmediateDeviceContext();

	deviceContextInst->setVertexShader(this->vertexShader);
	deviceContextInst->setPixelShader(this->pixelShader);
	constantBufferData cbData = {};

	if (this->deltaPos > 1.0f) {
		this->deltaPos = 0.0f;
	}
	else {
		this->deltaPos += this->deltaTime * 0.1f;
	}

	Matrix4x4 allMatrix;

	Matrix4x4 translationMatrix;
	translationMatrix.setTranslation(this->getLocalPosition());

	Matrix4x4 scaleMatrix;
	scaleMatrix.setScale(this->getLocalScale());

	Vector3D rotation = this->getLocalRotation();

	Matrix4x4 zMatrix, yMatrix, xMatrix;

	zMatrix.setRotationZ(rotation.z);
	xMatrix.setRotationX(rotation.x);
	yMatrix.setRotationY(rotation.y);

	Matrix4x4 rotationMatrix;

	rotationMatrix = xMatrix * yMatrix * zMatrix;
	allMatrix *= rotationMatrix;
	allMatrix *= translationMatrix;

	cbData.m_world = allMatrix;
	cbData.m_view.setIdentity();
	cbData.m_proj.setOrthogonalProjectionMatrix(width / 200.0f, height / 200.0f, -4.0f, 4.0f);

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
	vertex vertexList[] = {
		//X - Y - Z
		/***************FRONT FACE****************/
		{Vector3D(-0.5f, -0.5f, -0.5f), //POS1
			Vector3D(1,0,0), Vector3D(0.2f,0,0)},
		{Vector3D(-0.5f, 0.5f, -0.5f),    //POS2
			Vector3D(1,1,0), Vector3D(0.2f,0.2f,0)},
		{Vector3D(0.5f, 0.5f, -0.5f),    //POS3
			Vector3D(1,1,0), Vector3D(0.2f,0.2f,0)},
		{Vector3D(0.5f, -0.5f, -0.5f),     //POS4
			Vector3D(1,0,0), Vector3D(0.2f,0,0)},
		/******************************************/

		/***************BACK FACE****************/
		{Vector3D(0.5f, -0.5f, 0.5f), //POS1
			Vector3D(0,1,0), Vector3D(0,0.2f,0)},
		{Vector3D(0.5f, 0.5f, 0.5f),    //POS2
			Vector3D(0,1,1), Vector3D(0,0.2f,0.2f)},
		{Vector3D(-0.5f, 0.5f, 0.5f),    //POS3
			Vector3D(0,1,1), Vector3D(0,0.2f,0.2f)},
		{Vector3D(-0.5f, -0.5f, 0.5f),     //POS4
			Vector3D(0,1,0), Vector3D(0,0.2f,0)},
		/******************************************/
	};

	this->vertexBuffer = GraphicsEngine::getInstance()->createVertexBuffer();
	this->vertexBuffer->load(vertexList, sizeof(vertex), ARRAYSIZE(vertexList), shaderByteCode, sizeShader);

	/*----------------INDEX BUFFER PART----------------*/
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
	/*------------------------------------------------*/

	/*----------------VERTEX SHADER PART----------------*/
	GraphicsEngine::getInstance()->compileVertexShader(L"VertexShader.hlsl", "main", &shaderByteCode, &sizeShader);

	this->vertexShader = GraphicsEngine::getInstance()->createVertexShader(shaderByteCode, sizeShader);

	this->vertexBuffer->load(vertexList, sizeof(vertex), ARRAYSIZE(vertexList), shaderByteCode, sizeShader);

	GraphicsEngine::getInstance()->releaseCompiledShader();
	/*------------------------------------------------*/


	/*----------------PIXEL SHADER PART----------------*/
	GraphicsEngine::getInstance()->compilePixelShader(L"PixelShader.hlsl", "main", &shaderByteCode, &sizeShader);

	this->pixelShader = GraphicsEngine::getInstance()->createPixelShader(shaderByteCode, sizeShader);

	GraphicsEngine::getInstance()->releaseCompiledShader();
	/*------------------------------------------------*/
	/*----------------CONSTANT BUFFER PART----------------*/
	constantBufferData cc;
	cc.m_time = 0;

	this->constantBuffer = GraphicsEngine::getInstance()->createConstantBuffer();
	this->constantBuffer->load(&cc, sizeof(constantBufferData));
	/*------------------------------------------------*/
}
