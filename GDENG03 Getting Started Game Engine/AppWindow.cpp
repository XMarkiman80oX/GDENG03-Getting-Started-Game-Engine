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
#include "System.h"

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

	GraphicsEngine::getInstance()->registerComponent<TransformComponent>();
	GraphicsEngine::getInstance()->registerComponent<RenderComponent>();

	this->renderSystemECS = GraphicsEngine::getInstance()->registerSystem<System>(); // Register a generic system for rendering

	std::bitset<MAX_COMPONENTS> renderSignature;
	renderSignature.set(GraphicsEngine::getInstance()->getComponentType<TransformComponent>());
	renderSignature.set(GraphicsEngine::getInstance()->getComponentType<RenderComponent>());
	GraphicsEngine::getInstance()->setSystemSignature<System>(renderSignature); // Set signature for the generic system

	// Create a cube entity
	GameObjectManager::getInstance()->createGameObject(new Cube("Marco's Cube", shader_byte_code, size_shader, L"..\\Assets\\Textures\\brick.png", GraphicsEngine::getInstance()->getRenderSystem()));

	RECT rc = this->getClientWindowRect();
	this->m_swap_chain = GraphicsEngine::getInstance()->getRenderSystem()->createSwapChain(this->m_hwnd, rc.right - rc.left /* Width */, rc.bottom - rc.top /* Height */);
	this->m_depth_buffer = GraphicsEngine::getInstance()->getRenderSystem()->createDepthBuffer(rc.right - rc.left /* Width */, rc.bottom - rc.top /* Height */);


	WorldCamera::getInstance()->initialize(rc);
	WorldCamera::getInstance()->setTranslation(Vector3D(0, 0, -2));
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

	GraphicsEngine::getInstance()->getRenderSystem()->draw(rc.right - rc.left, rc.bottom - rc.top, *this->renderSystemECS);

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
}

void AppWindow::destroyGameObjects()
{
}

void AppWindow::selectNextObject()
{
	this->objectSelectedIndex++;

	//Make sure it doesn't go over the amount of objects in this->objectsInWorld
	if (this->objectSelectedIndex > 0)
		this->objectSelectedIndex = 0;

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