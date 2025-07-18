#include "Cube.h"
#include "SwapChain.h"
#include "WorldCamera.h"
#include "InputSystem.h"

Cube::Cube(std::string name, void* shaderByteCode, size_t sizeShader, const wchar_t* texturePath, RenderSystem* renderSystem)
	: BaseGameObject(name, renderSystem, Vector3D(),Vector3D(), Vector3D(), texturePath)
{
	this->initializeObject(shaderByteCode, sizeShader);
}

Cube::~Cube()
{
}

void Cube::update(RECT windowRect)
{
	// This function is now only responsible for updating animation variables if needed.
	// The constant buffer update is moved to the draw call for correctness.
	float movementRate = 1.0f / 0.55f;
	this->deltaPos += deltaTime * movementRate;

	if (this->deltaPos > 1.0f)
		deltaPos = 0;
}
void Cube::draw(int width, int height)
{
	DeviceContextPtr deviceContextInst = GraphicsEngine::getInstance()->getRenderSystem()->getImmediateDeviceContext();

	deviceContextInst->setVertexShader(this->vertexShader);
	deviceContextInst->setPixelShader(this->pixelShader);
	deviceContextInst->setTexture(this->pixelShader, this->texture);
	constantBufferData cbData = {};

	// Set the time for animations
	cbData.m_time = static_cast<unsigned int>(EngineTime::getTotalElapsedTime() * 1000.0);

	if (this->deltaPos > 1.0f) 
		this->deltaPos = 0.0f;

	else 
		this->deltaPos += this->deltaTime * 0.1f;
	

	Matrix4x4 allMatrix;
	allMatrix.setIdentity(); // Initialize the matrix

	Matrix4x4 translationMatrix;
	translationMatrix.setTranslation(this->getLocalPosition());

	Matrix4x4 scaleMatrix;
	scaleMatrix.setScale(this->cubeScale);

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

	// Apply transformations: Scale -> Rotate -> Translate
	allMatrix *= scaleMatrix;
	allMatrix *= rotationMatrix;
	allMatrix *= translationMatrix;

	cbData.m_world = allMatrix;

	// Use the WorldCamera's view and projection matrices
	cbData.m_view = WorldCamera::getInstance()->getViewMatrix();
	cbData.m_proj = WorldCamera::getInstance()->getProjectionMatrix(); // Corrected this line

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
	InputSystem::getInstance()->addListener(this);

	this->setTexture(L"..\\Assets\\Textures\\brick.png");
	//Set the vertices of the object here
	//This is using the triangle strip approach
	Vector3D position_list[] =
	{
		/***************FRONT FACE****************/
		{Vector3D(-0.5f, -0.5f, -0.5f) }, //POS1
		{Vector3D(-0.5f, 0.5f, -0.5f) },    //POS2
		{Vector3D(0.5f, 0.5f, -0.5f) },    //POS3
		{Vector3D(0.5f, -0.5f, -0.5f) },     //POS4
		/******************************************/

		/***************BACK FACE****************/
		{Vector3D(0.5f, -0.5f, 0.5f)}, //POS1
		{Vector3D(0.5f, 0.5f, 0.5f) },    //POS2
		{Vector3D(-0.5f, 0.5f, 0.5f) },    //POS3
		{Vector3D(-0.5f, -0.5f, 0.5f) }    //POS4
		/******************************************/
	};
	Vector2D texcoord_list[] =
	{
		{Vector2D(0.0f)},
		{Vector2D(0.0f, 1.0f) },
		{Vector2D(1.0f,0.0f)},
		{Vector2D(1.0f)}

	};
	vertex vertexList[] = {
		{position_list[0], texcoord_list[1]},
		{position_list[1], texcoord_list[0]},
		{position_list[2], texcoord_list[2]},
		{position_list[3], texcoord_list[3]},

		{position_list[4], texcoord_list[1]},
		{position_list[5], texcoord_list[0]},
		{position_list[6], texcoord_list[2]},
		{position_list[7], texcoord_list[3]},

		{position_list[1], texcoord_list[1]},
		{position_list[6], texcoord_list[0]},
		{position_list[5], texcoord_list[2]},
		{position_list[2], texcoord_list[3]},

		{position_list[7], texcoord_list[1]},
		{position_list[0], texcoord_list[0]},
		{position_list[3], texcoord_list[2]},
		{position_list[4], texcoord_list[3]},

		{position_list[3], texcoord_list[1]},
		{position_list[2], texcoord_list[0]},
		{position_list[5], texcoord_list[2]},
		{position_list[4], texcoord_list[3]},

		{position_list[7], texcoord_list[1]},
		{position_list[6], texcoord_list[0]},
		{position_list[1], texcoord_list[2]},
		{position_list[0], texcoord_list[3]}
	};

	/*----------------INDEX BUFFER PART----------------*/
	unsigned int index_list[] = {
		//FRONT SIDE
		0,1,2, // 1st Triangle
		2,3,0, //2nd Triangle
		//BACK SIDE
		4,5,6,
		6,7,4,
		//TOP SIDE
		8,9,10,
		10,11,8,
		//BOTTOM SIDE
		12,13,14,
		14,15,12,
		//RIGHT SIDE
		16,17,18,
		18,19,16,
		//LEFT SIDE
		20,21,22,
		22,23,20
	};
	
	UINT size_index_list = ARRAYSIZE(index_list); 
	this->indexBuffer = GraphicsEngine::getInstance()->getRenderSystem()->createIndexBuffer(index_list, size_index_list);

	/*----------------VERTEX SHADER PART----------------*/
	GraphicsEngine::getInstance()->getRenderSystem()->compileVertexShader(L"VertexShader.hlsl", "main", &shaderByteCode, &sizeShader);

	this->vertexShader = GraphicsEngine::getInstance()->getRenderSystem()->createVertexShader(shaderByteCode, sizeShader);
	this->vertexBuffer = GraphicsEngine::getInstance()->getRenderSystem()->createVertexBuffer(vertexList, sizeof(vertex), ARRAYSIZE(vertexList), shaderByteCode, sizeShader);

	GraphicsEngine::getInstance()->getRenderSystem()->releaseCompiledShader();
	/*------------------------------------------------*/


	/*----------------PIXEL SHADER PART----------------*/
	GraphicsEngine::getInstance()->getRenderSystem()->compilePixelShader(L"PixelShader.hlsl", "main", &shaderByteCode, &sizeShader);

	this->pixelShader = GraphicsEngine::getInstance()->getRenderSystem()->createPixelShader(shaderByteCode, sizeShader);

	GraphicsEngine::getInstance()->getRenderSystem()->releaseCompiledShader();
	/*------------------------------------------------*/


	/*----------------CONSTANT BUFFER PART----------------*/
	constantBufferData cc;
	cc.m_time = 0;

	this->constantBuffer = GraphicsEngine::getInstance()->getRenderSystem()->createConstantBuffer(&cc, sizeof(constantBufferData));

	/*------------------------------------------------*/
}

void Cube::onKeyDown(int key)
{
}

void Cube::onKeyUp(int key)
{
}

void Cube::onMouseMove(const Point& mousePosition)
{
}

void Cube::onLeftMouseDown(const Point& mousePosition)
{
	this->cubeScale = Vector3D(0.5f);
}

void Cube::onLeftMouseUp(const Point& mousePosition)
{
	this->cubeScale = Vector3D(1.0f);
}

void Cube::onRightMouseDown(const Point& mousePosition)
{
	this->cubeScale = Vector3D(2.0f);
}

void Cube::onRightMouseUp(const Point& mousePosition)
{
	this->cubeScale = Vector3D(1.0f);
}

