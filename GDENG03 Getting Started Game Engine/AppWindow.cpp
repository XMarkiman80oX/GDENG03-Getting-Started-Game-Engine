#include "AppWindow.h"
#include <Windows.h>
#include "Vector3D.h"
#include "Vector2D.h"
#include "Matrix4x4.h"
#include "InputSystem.h"
#include "Mesh.h"
#include "Cube.h"
#include "Sphere.h"
#include "Plane.h"
#include "ConstantBufferData.h"
#include "TransformComponent.h"
#include "RenderComponent.h"

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
	void* shader_byte_code = nullptr;
	size_t size_shader = 0;

	//We need to add AppWindow as a listener to the Input System
	InputSystem::getInstance()->addListener(this);
	InputSystem::getInstance()->showCursor(this->cursorIsVisible);

	GraphicsEngine::getInstance()->initECS();
	GraphicsEngine::getInstance()->registerComponent<TransformComponent>();
	GraphicsEngine::getInstance()->registerComponent<RenderComponent>();

	auto renderSystemECS = GraphicsEngine::getInstance()->registerSystem<System>(); // Register a generic system for rendering

	std::bitset<MAX_COMPONENTS> renderSignature;
	renderSignature.set(GraphicsEngine::getInstance()->getComponentType<TransformComponent>());
	renderSignature.set(GraphicsEngine::getInstance()->getComponentType<RenderComponent>());
	GraphicsEngine::getInstance()->setSystemSignature<System>(renderSignature); // Set signature for the generic system

	// Create a cube entity
	EntityId cube = GraphicsEngine::getInstance()->createEntity();

	TransformComponent transform;
	transform.position = Vector3D(0, 0, 0);
	transform.rotation = Vector3D(0, 0, 0);
	transform.scale = Vector3D(1, 1, 1);
	GraphicsEngine::getInstance()->addComponent(cube, transform);;

	RenderComponent render;
	render.mesh = GraphicsEngine::getInstance()->getMeshManager()->createMeshFromFile(L"..\\Assets\\Meshes\\cube.obj");
	render.texture = GraphicsEngine::getInstance()->getTextureManager()->createTextureFromFile(L"..\\Assets\\Textures\\brick.png");
	
	// Create a basic material
	GraphicsEngine::getInstance()->getRenderSystem()->compileVertexShader(L"VertexShader.hlsl", "main", &shader_byte_code, &size_shader);
	VertexShaderPtr vs = GraphicsEngine::getInstance()->getRenderSystem()->createVertexShader(shader_byte_code, size_shader);
	GraphicsEngine::getInstance()->getRenderSystem()->releaseCompiledShader();

	GraphicsEngine::getInstance()->getRenderSystem()->compilePixelShader(L"PixelShader.hlsl", "main", &shader_byte_code, &size_shader);
	PixelShaderPtr ps = GraphicsEngine::getInstance()->getRenderSystem()->createPixelShader(shader_byte_code, size_shader);
	GraphicsEngine::getInstance()->getRenderSystem()->releaseCompiledShader();

	constantBufferData cbd = {};
	cbd.m_time = 0;
	ConstantBufferPtr cb = GraphicsEngine::getInstance()->getRenderSystem()->createConstantBuffer(&cbd, sizeof(constantBufferData));


	GraphicsEngine::getInstance()->addComponent(cube, render);

	/*try {
		this->m_mesh = GraphicsEngine::getInstance()->getMeshManager()->createMeshFromFile(L"..\\Assets\\Meshes\\teapot.obj");
		this->m_mesh2 = GraphicsEngine::getInstance()->getMeshManager()->createMeshFromFile(L"..\\Assets\\Meshes\\bunny.obj");
		this->m_mesh3 = GraphicsEngine::getInstance()->getMeshManager()->createMeshFromFile(L"..\\Assets\\Meshes\\armadillo.obj");
	}
	catch (const std::exception& e) {
		MessageBox(nullptr, L"Failed to load mesh.", L"Error", MB_OK);
	}*/

	RECT rc = this->getClientWindowRect();
	this->m_swap_chain = GraphicsEngine::getInstance()->getRenderSystem()->createSwapChain(this->m_hwnd, rc.right - rc.left /* Width */, rc.bottom - rc.top /* Height */);
	this->m_depth_buffer = GraphicsEngine::getInstance()->getRenderSystem()->createDepthBuffer(rc.right - rc.left /* Width */, rc.bottom - rc.top /* Height */);


	WorldCamera::getInstance()->initialize(rc);
	WorldCamera::getInstance()->setTranslation(Vector3D(0, 0, -2));

	/*Cube* marcosCube = new Cube("Marco's Cube", shader_byte_code, size_shader, GraphicsEngine::getInstance()->getRenderSystem());
	Plane* marcosPlane = new Plane("Marco's Plane", shader_byte_code, size_shader, GraphicsEngine::getInstance()->getRenderSystem());

	marcosPlane->setPosition(Vector3D(0, 0, 2));
	marcosPlane->setScale(100);
	this->objectsInWorld.push_back(marcosCube);
	this->objectsInWorld.push_back(marcosPlane);*/
}

void AppWindow::onUpdate()
{
	InputSystem::getInstance()->update();

	this->deltaTime = static_cast<float>(EngineTime::getDeltaTime());

	GraphicsEngine::getInstance()->getRenderSystem()->getImmediateDeviceContext()->clearRenderTargetColor
	(
		this->m_swap_chain,
		this->m_depth_buffer,
		0, 0.3f, 0.4f, 1
	);
	RECT rc = this->getClientWindowRect();
	GraphicsEngine::getInstance()->getRenderSystem()->getImmediateDeviceContext()->setViewportSize(rc.right - rc.left, rc.bottom - rc.top);

	WorldCamera::getInstance()->updateCamera();

	auto renderSystemECS = GraphicsEngine::getInstance()->registerSystem<System>();

	GraphicsEngine::getInstance()->getRenderSystem()->draw(rc.right - rc.left, rc.bottom - rc.top, *renderSystemECS);

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