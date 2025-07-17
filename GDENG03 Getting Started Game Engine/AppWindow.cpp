#include "AppWindow.h"
#include <Windows.h>
#include "Vector3D.h"
#include "Vector2D.h"
#include "Matrix4x4.h"
#include "InputSystem.h"
#include "Mesh.h"

struct vertex
{
	Vector3D position;
	Vector2D texcoord; //Texture coordinates	
};
/*
	"DirectX handles the constant in video memory in checks of 16 bytes, so if our structure has
	a size of 24 bytes, this size must be modified to be a multiple of 16, so we will have to enlarge it
	of further 8 bytes. That's what "__declspec(align(16))" does.
*/
__declspec(align(16))
struct constant
{
	Matrix4x4 m_world;
	Matrix4x4 m_view;
	Matrix4x4 m_proj;

	unsigned int m_time;
};

AppWindow::AppWindow()
{
}

AppWindow::~AppWindow()
{
}

void AppWindow::onCreate()
{
	//We need to add AppWindow as a listener to the Input System
	InputSystem::getInstance()->addListener(this);
	InputSystem::getInstance()->showCursor(this->cursorIsVisible);

	try {
		this->m_wood_tex = GraphicsEngine::get()->getTextureManager()->createTextureFromFile(L"..\\Assets\\Textures\\brick.png");
	}
	catch (const std::exception& e) {
		// Handle the exception, e.g., show an error message
		MessageBox(nullptr, L"Failed to load texture.", L"Error", MB_OK);
		// You might want to close the application here or use a default texture
	}
	try {
		this->m_mesh = GraphicsEngine::get()->getMeshManager()->createMeshFromFile(L"..\\Assets\\Meshes\\teapot.obj");
	}
	catch (const std::exception& e) {
		// Handle the exception, e.g., show an error message
		MessageBox(nullptr, L"Failed to load mesh.", L"Error", MB_OK);
		// You might want to close the application here or use a default texture
	}
	
	RECT rc = this->getClientWindowRect();
	this->m_swap_chain = GraphicsEngine::get()->getRenderSystem()->createSwapChain(this->m_hwnd, rc.right - rc.left /* Width */, rc.bottom - rc.top /* Height */);
	this->m_depth_buffer = GraphicsEngine::get()->getRenderSystem()->createDepthBuffer(rc.right - rc.left /* Width */, rc.bottom - rc.top /* Height */);


	this->worldCamera.setTranslation(Vector3D(0,0,-2));

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
	UINT size_list = ARRAYSIZE(vertexList);
	
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
	this->m_index_buffer = GraphicsEngine::get()->getRenderSystem()->createIndexBuffer(index_list, size_index_list);
	
	/*------------------------------------------------*/

	/*----------------VERTEX SHADER PART----------------*/
	//Here we create the vertex buffer, then the established vertex list will be loaded here later on
	void* shader_byte_code = nullptr;
	size_t size_shader = 0;
	GraphicsEngine::get()->getRenderSystem()->compileVertexShader(L"VertexShader.hlsl", "main", &shader_byte_code, &size_shader);
	

	this->m_vertex_shader = GraphicsEngine::get()->getRenderSystem()->createVertexShader(shader_byte_code, size_shader);

	this->m_vertex_buffer = GraphicsEngine::get()->getRenderSystem()->createVertexBuffer(vertexList, sizeof(vertex), size_list, shader_byte_code, size_shader);
	
	GraphicsEngine::get()->getRenderSystem()->releaseCompiledShader();
	/*------------------------------------------------*/

	/*----------------PIXEL SHADER PART----------------*/
	GraphicsEngine::get()->getRenderSystem()->compilePixelShader(L"PixelShader.hlsl", "main", &shader_byte_code, &size_shader);

	this->m_pixel_shader = GraphicsEngine::get()->getRenderSystem()->createPixelShader(shader_byte_code, size_shader);

	GraphicsEngine::get()->getRenderSystem()->releaseCompiledShader();
	/*------------------------------------------------*/

	/*----------------CONSTANT BUFFER PART----------------*/
	constant cc;
	cc.m_time = 0;

	this->m_constant_buffer = GraphicsEngine::get()->getRenderSystem()->createConstantBuffer(&cc, sizeof(constant));
	/*------------------------------------------------*/

}

void AppWindow::onUpdate()
{
	//change color here
	//Inputs get processed here
	InputSystem::getInstance()->update();

	GraphicsEngine::get()->getRenderSystem()->getImmediateDeviceContext()->clearRenderTargetColor
	(
		this->m_swap_chain, 
		this->m_depth_buffer, 
		0, 0.3f, 0.4f, 1
	);
	RECT rc = this->getClientWindowRect();
	GraphicsEngine::get()->getRenderSystem()->getImmediateDeviceContext()->setViewportSize(rc.right - rc.left, rc.bottom - rc.top);

	this->update();

	GraphicsEngine::get()->getRenderSystem()->getImmediateDeviceContext()->setConstantBuffer(this->m_vertex_shader, this->m_constant_buffer);
	GraphicsEngine::get()->getRenderSystem()->getImmediateDeviceContext()->setConstantBuffer(this->m_pixel_shader, this->m_constant_buffer);

	GraphicsEngine::get()->getRenderSystem()->getImmediateDeviceContext()->setVertexShader(this->m_vertex_shader);
	GraphicsEngine::get()->getRenderSystem()->getImmediateDeviceContext()->setPixelShader(this->m_pixel_shader);

	GraphicsEngine::get()->getRenderSystem()->getImmediateDeviceContext()->setTexture(this->m_pixel_shader, this->m_wood_tex);

	//Here we will pass the vertex buffer from which to get the vertices to render
    GraphicsEngine::get()->getRenderSystem()->getImmediateDeviceContext()->setVertexBuffer(this->m_mesh->getVertexBuffer());

	GraphicsEngine::get()->getRenderSystem()->getImmediateDeviceContext()->setIndexBuffer(this->m_mesh->getIndexBuffer());

	GraphicsEngine::get()->getRenderSystem()->getImmediateDeviceContext()->drawIndexedTriangleList(this->m_mesh->getIndexBuffer()->getSizeIndexList(), 0, 0);
	m_swap_chain->present(true);

	this->m_old_delta = this->m_new_delta;
	this->m_new_delta = ::GetTickCount();

	//if the old delta has no value, set it to 0 so we dont have a new delta that equals to the new delta one
	this->m_delta_time = (this->m_old_delta) ? ((this->m_new_delta - this->m_old_delta) / 1000.0f) : 0;
}

void AppWindow::onDestroy()
{
	Window::onDestroy();
}

void AppWindow::onFocus()
{
	InputSystem::getInstance()->addListener(this);
}

void AppWindow::onKillFocus()
{
	InputSystem::getInstance()->removeListener(this);
}

void AppWindow::update()
{
	constant cc;
	cc.m_time = ::GetTickCount();/*This is a windows function that allows us to get the time elapsed since the
								system started in milliseconds*/

	float movementRate = 1.0f / 0.55f;
	//This means we reach one unit per 1/movementRate seconds (reciprocal)
	this->m_delta_pos += m_delta_time * movementRate;

	if (this->m_delta_pos > 1.0f)
		m_delta_pos = 0;

	Matrix4x4 temp;
	this->m_delta_scale += this->m_delta_time * movementRate;
	//cc.m_world.setTranslation(Vector3D::lerp(Vector3D(-2.0f, -2.0f, 0.0f), Vector3D(2.0f, 2.0f, 0.0f), this->m_delta_pos));
	/*cc.m_world.setScale(Vector3D::lerp(Vector3D(0.5f, 0.5f, 0.0f), Vector3D(2.0f, 2.0f, 0.0f), (sin(this->m_delta_scale)+1.0f)/2.0f));

	temp.setTranslation(Vector3D::lerp(Vector3D(-1.5f, -1.5f, 0.0f), Vector3D(1.5f, 1.5f, 0.0f), this->m_delta_pos));

	cc.m_world *= temp;*/

	//cc.m_world.setScale(this->cubeScale);

	//temp.setIdentity();
	//temp.setRotationZ(0.0f);
	//cc.m_world *= temp;

	//temp.setIdentity();
	//temp.setRotationY(this->rotationY);
	//cc.m_world *= temp;

	//temp.setIdentity();
	//temp.setRotationX(this->rotationX);
	//cc.m_world *= temp;

	cc.m_world.setIdentity();
	float cubeSizeMultiplier = 1 / 300.0f;

	Matrix4x4 worldCam;
	worldCam.setIdentity();

	temp.setIdentity();
	temp.setRotationX(rotationX);
	worldCam *= temp;

	temp.setIdentity();
	temp.setRotationY(rotationY);
	worldCam *= temp;

	//moving through the z axis
	//float value entails how much units is moved
	Vector3D newPos = this->worldCamera.getTranslation() + worldCam.getZDirection() * (this->forward*0.3f);
	
	newPos = newPos + worldCam.getXDirection() * (this->rightward*0.3f);

	//setting our camera backwards two points along the x axis
	worldCam.setTranslation(newPos);
	this->worldCamera = worldCam;
	worldCam.setInverse();

	cc.m_view = worldCam;
	cc.m_proj.setOrthogonalProjectionMatrix(
		(this->getClientWindowRect().right - this->getClientWindowRect().left) * cubeSizeMultiplier,
		(this->getClientWindowRect().bottom - this->getClientWindowRect().top) * cubeSizeMultiplier,
		-4.0f,
		4.0f
	);
	int width = (this->getClientWindowRect().right - this->getClientWindowRect().left);
	int height = (this->getClientWindowRect().bottom - this->getClientWindowRect().top);
	
	cc.m_proj.setPerspectiveFOVLH(1.57f, (float)width/(float)height, 0.1f, 100.0f);
	this->m_constant_buffer->update(GraphicsEngine::get()->getRenderSystem()->getImmediateDeviceContext(), &cc);
}

void AppWindow::onLeftMouseDown(const Point& mousePosition)
{
	//means xyz are all uniform
	this->cubeScale = Vector3D(0.5f);
}

void AppWindow::onLeftMouseUp(const Point& mousePosition)
{
	this->cubeScale = Vector3D(1.0f);
}

void AppWindow::onRightMouseDown(const Point& mousePosition)
{
	this->cubeScale = Vector3D(2.0f);
}

void AppWindow::onRightMouseUp(const Point& mousePosition)
{
	this->cubeScale = Vector3D(1.0f);
}

void AppWindow::onMouseMove(const Point& mousePosition)
{
	int width = (this->getClientWindowRect().right - this->getClientWindowRect().left);
	int height = (this->getClientWindowRect().bottom - this->getClientWindowRect().top);
	
	float incrementerX = this->rotationSpeedMultiplier * (mousePosition.x - (width/2.0f)) * this->m_delta_time;
	float incrementerY = this->rotationSpeedMultiplier * (mousePosition.y - (height/2.0f)) * this->m_delta_time;
	
	if (!this->invertedIsOn)
	{
		this->rotationX -= incrementerY;
		this->rotationY -= incrementerX;
	}
	else 
	{
		this->rotationX += incrementerY;
		this->rotationY += incrementerX;
	}
	
	//So it clamps to the mouse's initial position in the window
	InputSystem::getInstance()->setCursorPosition(Point(width/2.0f, height/2.0f));
}

void AppWindow::onKeyDown(int key)
{
	switch (key) {
	case 'W':
		//this->rotationX += rotationSpeedMultiplier * this->m_delta_time;
		this->forward = 1.0f;
		break;
	case 'S':
		//this->rotationX -= rotationSpeedMultiplier * this->m_delta_time;
		this->forward = -1.0f;
		break;
	case 'A':
		//this->rotationY += rotationSpeedMultiplier * this->m_delta_time;
		this->rightward = -1.0f;
		break;
	case 'D':
		//this->rotationY -= rotationSpeedMultiplier * this->m_delta_time;
		this->rightward = 1.0f;
		break;
	}
}

void AppWindow::onKeyUp(int key)
{
	//0.0f since we want to stop our camera
	this->forward = 0.0f;
	this->rightward = 0.0f;
}