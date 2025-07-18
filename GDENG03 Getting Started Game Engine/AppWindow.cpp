#include "AppWindow.h"
#include <Windows.h>
#include "Vector3D.h"
#include "Vector2D.h"
#include "Matrix4x4.h"
#include "InputSystem.h"
#include "Mesh.h"
#include "Cube.h"
#include "Sphere.h"

struct vertex
{
	Vector3D position;
	Vector2D texcoord; //Texture coordinates
};

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

AppWindow* AppWindow::getInstance()
{
	static AppWindow appWindow;
	return &appWindow;
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
		MessageBox(nullptr, L"Failed to load texture.", L"Error", MB_OK);
	}
	try {
		this->m_mesh = GraphicsEngine::get()->getMeshManager()->createMeshFromFile(L"..\\Assets\\Meshes\\teapot.obj");
		this->m_mesh2 = GraphicsEngine::get()->getMeshManager()->createMeshFromFile(L"..\\Assets\\Meshes\\bunny.obj");
		this->m_mesh3 = GraphicsEngine::get()->getMeshManager()->createMeshFromFile(L"..\\Assets\\Meshes\\armadillo.obj");
	}
	catch (const std::exception& e) {
		MessageBox(nullptr, L"Failed to load mesh.", L"Error", MB_OK);
	}

	RECT rc = this->getClientWindowRect();
	this->m_swap_chain = GraphicsEngine::get()->getRenderSystem()->createSwapChain(this->m_hwnd, rc.right - rc.left /* Width */, rc.bottom - rc.top /* Height */);
	this->m_depth_buffer = GraphicsEngine::get()->getRenderSystem()->createDepthBuffer(rc.right - rc.left /* Width */, rc.bottom - rc.top /* Height */);


	this->worldCamera.setTranslation(Vector3D(0, 0, -2));

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
	//Inputs get processed here
	InputSystem::getInstance()->update();

	this->deltaTime = static_cast<float>(EngineTime::getDeltaTime());

	GraphicsEngine::get()->getRenderSystem()->getImmediateDeviceContext()->clearRenderTargetColor
	(
		this->m_swap_chain,
		this->m_depth_buffer,
		0, 0.3f, 0.4f, 1
	);
	RECT rc = this->getClientWindowRect();
	GraphicsEngine::getInstance()->getImmediateDeviceContext()->setViewportSize(rc.right - rc.left, rc.bottom - rc.top);


	WorldCamera::getInstance()->updateCamera();

	this->updateGameObjects(rc);

	m_swap_chain->present(true);
}

void AppWindow::onDestroy()
{
	Window::onDestroy();
}

void AppWindow::onFocus()
{
	InputSystem::getInstance()->addListener(this);
	InputSystem::getInstance()->addListener(WorldCamera::getInstance());
}

void AppWindow::onKillFocus()
{
	InputSystem::getInstance()->removeListener(this);
	InputSystem::getInstance()->removeListener(WorldCamera::getInstance());
}


void AppWindow::onLeftMouseDown(const Point& mousePosition)
{
}

void AppWindow::onLeftMouseUp(const Point& mousePosition)
{
}

void AppWindow::onRightMouseDown(const Point& mousePosition)
{
}

void AppWindow::onRightMouseUp(const Point& mousePosition)
{
}

void AppWindow::updateGameObjects(RECT clientWindowRect)
{
	for(BaseGameObject* object : this->objectsInWorld)
	{
		object->update(clientWindowRect);
		object->draw(clientWindowRect.right - clientWindowRect.left, clientWindowRect.bottom - clientWindowRect.top);
	}
}

void AppWindow::destroyGameObjects()
{
	this->objectsInWorld.clear();
}

void AppWindow::selectNextObject()
{
	this->objectSelectedIndex++;

	//Make sure it doesn't go over the amount of objects in this->objectsInWorld
	if (this->objectSelectedIndex > this->objectsInWorld.size() - 1)
		this->objectSelectedIndex = 0;

	//Set All Selected Objects to false first before toggling it on
	for (BaseGameObject* object : this->objectsInWorld) {
		this->objectsInWorld[this->objectSelectedIndex]->setSelected(false);
	}

	this->objectsInWorld[this->objectSelectedIndex]->setSelected(true);
}

void AppWindow::onMouseMove(const Point& mousePosition)
{
}

void AppWindow::onKeyDown(int key)
{
}

void AppWindow::onKeyUp(int key)
{
	switch (key) {
	case VK_SPACE:
		this->selectNextObject();
		break;
	}
}