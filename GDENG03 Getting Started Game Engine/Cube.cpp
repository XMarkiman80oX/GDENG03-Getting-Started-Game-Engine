#include "Cube.h"
#include "GraphicsEngine.h"
#include "SwapChain.h"

Cube::Cube(std::string name, void* shaderByteCode, size_t sizeShader) : BaseGameObject(name)
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

	/*----------------CONSTANT BUFFER PART----------------*/
	constantBufferData cc;
	cc.m_time = 0;

	this->constantBuffer = GraphicsEngine::getInstance()->createConstantBuffer();
	this->constantBuffer->load(&cc, sizeof(constantBufferData));
	/*------------------------------------------------*/
}

Cube::~Cube()
{
	this->vertexBuffer->release();
	this->indexBuffer->release();
	this->constantBuffer->release();
	BaseGameObject::~BaseGameObject();
}

void Cube::update(float deltaTime)
{
	this->deltaTime = deltaTime;

	Vector3D currentRotation = this->getLocalRotation();
	currentRotation.x += m_rotation_speed.x * deltaTime;
	currentRotation.y += m_rotation_speed.y * deltaTime;
	currentRotation.z += m_rotation_speed.z * deltaTime;
	this->setRotation(currentRotation);
}

void Cube::draw(int width, int height, VertexShader* vertexShader, PixelShader* pixelShader)
{
	DeviceContext* deviceContextInst = GraphicsEngine::getInstance()->getImmediateDeviceContext();

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