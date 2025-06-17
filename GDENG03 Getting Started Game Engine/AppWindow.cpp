#include "AppWindow.h"
#include <Windows.h>
#include "Vector3D.h"
#include "Matrix4x4.h"
#include "InputSystem.h"
#include "Cube.h"


struct vertex
{
	Vector3D position;
	Vector3D color;
	Vector3D color1;
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

AppWindow* AppWindow::getInstance() 
{
	static AppWindow appWindow;
	return &appWindow;
}

RECT AppWindow::getScreenSize()
{
	return this->getClientWindowRect();
}

void AppWindow::onCreate()
{
	void* shader_byte_code = nullptr;
	size_t size_shader = 0;
	//Window::onCreate();
	//We need to add AppWindow as a listener to the Input System
	InputSystem::getInstance()->addListener(this);
	InputSystem::getInstance()->showCursor(this->cursorIsVisible);

	GraphicsEngine::getInstance()->init();
	this->m_swap_chain = GraphicsEngine::getInstance()->createSwapChain();

	RECT rc = this->getClientWindowRect();
	this->m_swap_chain->init(this->m_hwnd, rc.right - rc.left, rc.bottom - rc.top);

	// Create and initialize the depth buffer
	m_depth_buffer = new DepthBuffer();
	m_depth_buffer->init(rc.right - rc.left, rc.bottom - rc.top);

	WorldCamera::getInstance()->getWorldCamera().setTranslation(Vector3D(0, 0, -2));

	this->baseCube = new Cube("Marco's Cube", shader_byte_code, size_shader);

}

void AppWindow::onUpdate()
{
	//Window::onUpdate();
	//change color here
	//Inputs get processed here
	InputSystem::getInstance()->update();

	GraphicsEngine::getInstance()->getImmediateDeviceContext()->clearRenderTargetColor(this->m_swap_chain,
		this->m_depth_buffer, 0, 0.3f, 0.4f, 1);
	RECT rc = this->getClientWindowRect();
	GraphicsEngine::getInstance()->getImmediateDeviceContext()->setViewportSize(rc.right - rc.left, rc.bottom - rc.top);

	this->baseCube->update(this->m_delta_time, this->getClientWindowRect());

	this->baseCube->draw(rc.right - rc.left, rc.bottom - rc.top);

	
	m_swap_chain->present(true);

	this->m_old_delta = this->m_new_delta;
	this->m_new_delta = ::GetTickCount();

	//if the old delta has no value, set it to 0 so we dont have a new delta that equals to the new delta one
	this->m_delta_time = (this->m_old_delta) ? ((this->m_new_delta - this->m_old_delta) / 1000.0f) : 0;
}

void AppWindow::onDestroy()
{
	Window::onDestroy();
	this->m_vertex_buffer->release();
	this->m_index_buffer->release();
	this->m_constant_buffer->release();
	this->m_depth_buffer->release(); // Release the depth buffer
	this->m_swap_chain->release();
	this->m_vertex_shader->release();
	this->m_pixel_shader->release();
	delete this->baseCube;
	GraphicsEngine::getInstance()->release();
}

void AppWindow::onFocus()
{
	InputSystem::getInstance()->addListener(this);
}

void AppWindow::onKillFocus()
{
	InputSystem::getInstance()->removeListener(this);
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