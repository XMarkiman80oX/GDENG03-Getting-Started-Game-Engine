#include "AppWindow.h"
#include <Windows.h>
#include "Vector3D.h"
#include "Matrix4x4.h"
#include "InputSystem.h"
#include "Cube.h"
#include "Sphere.h"
#include <random>

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

	//WorldCamera::getInstance()->getWorldCamera().setTranslation(Vector3D(0, 0, -2));

	WorldCamera::getInstance()->initialize(rc);
	std::cout << "windowRect.right: " << rc.right;
	std::cout << ", windowRect.left " << rc.left << std::endl;

	std::cout << "windowRect.bottom: " << rc.bottom;
	std::cout << ", windowRect.top " << rc.top << std::endl;

	//Initialize Objects Here
	for (int i = 0; i < Config::NUMBER_OF_CUBES; i++)
	{
		Cube* marcosCube = new Cube("Marco's Cube " + i+1, shader_byte_code, size_shader);

		//Get Randomized x, y and z values
		float x = this->randomizeFromRange(-5.0f, 5.0f);
		float y = this->randomizeFromRange(-5.0f, 5.0f);
		float z = this->randomizeFromRange(-5.0f, 5.0f);

		if (!Config::RANDOMIZE_OBJECT_POSITIONS) {
			x = 0; y = 0; z = 0;
		}

		//Set Randomized Positions
		marcosCube->setPosition(x, y, z);
		marcosCube->getLocalPosition().printVector();
		//Place in vector
		this->objectsInWorld.push_back(marcosCube);
	}

	/*Cube* marcosCube = new Cube("Marco's Cube ", shader_byte_code, size_shader);
	Cube* marcosCube2 = new Cube("Marco's Cube 2", shader_byte_code, size_shader);
	Cube* marcosCube3 = new Cube("Marco's Cube 3", shader_byte_code, size_shader);

	marcosCube->setPosition(1.0f, 0.0f, 0.0f);
	marcosCube3->setPosition(0.5f, 1.0f, 0.0f);
	marcosCube2->setPosition(0.0f, 0.0f, 1.0f);

	this->objectsInWorld.push_back(marcosCube);
	this->objectsInWorld.push_back(marcosCube2);
	this->objectsInWorld.push_back(marcosCube3);*/

	this->initializeFirstAsSelected();

}

void AppWindow::onUpdate()
{
	//Inputs get processed here
	InputSystem::getInstance()->update();

	this->deltaTime = static_cast<float>(EngineTime::getDeltaTime());

	GraphicsEngine::getInstance()->getImmediateDeviceContext()->clearRenderTargetColor(this->m_swap_chain,
		this->m_depth_buffer, 0, 0.3f, 0.4f, 1);
	RECT rc = this->getClientWindowRect();
	GraphicsEngine::getInstance()->getImmediateDeviceContext()->setViewportSize(rc.right - rc.left, rc.bottom - rc.top);


	WorldCamera::getInstance()->updateCamera();

	this->updateGameObjects(rc);

	m_swap_chain->present(true);
}

void AppWindow::onDestroy()
{
	Window::onDestroy();
	this->m_depth_buffer->release(); // Release the depth buffer
	this->m_swap_chain->release();
	this->destroyGameObjects();
	GraphicsEngine::getInstance()->release();
}

void AppWindow::onFocus()
{
	InputSystem::getInstance()->addListener(this);

	if(Config::ENABLE_FPS_CAMERA)
		InputSystem::getInstance()->addListener(WorldCamera::getInstance());
}

void AppWindow::onKillFocus()
{
	InputSystem::getInstance()->removeListener(this);

	if (Config::ENABLE_FPS_CAMERA)
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
	if (!this->objectsInWorld.empty()) {

		for (BaseGameObject* object : this->objectsInWorld)
		{
			object->update(clientWindowRect);
			object->draw(clientWindowRect.right - clientWindowRect.left, clientWindowRect.bottom - clientWindowRect.top);
		}
	}
	else
		this->onDestroy();
}

void AppWindow::destroyGameObjects()
{
	this->objectsInWorld.clear();
}

void AppWindow::initializeFirstAsSelected()
{
	if(!this->objectsInWorld.empty())
		this->objectsInWorld[0]->setSelected(true);
	else
		this->onDestroy();
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
float AppWindow::randomizeFromRange(float lowerBound, float upperBound)
{
	std::random_device rd;
	std::mt19937 eng(rd());
	std::uniform_real_distribution<> randomizedFloat(lowerBound, upperBound);

	return static_cast<float> (randomizedFloat(eng));
}