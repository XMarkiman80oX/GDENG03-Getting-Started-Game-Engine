#include "AppWindow.h"
#include <Windows.h>
#include "Vector3D.h"
#include "Matrix4x4.h"
#include "InputSystem.h"
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

	if (!Config::RANDOMIZE_OBJECT_POSITIONS) {
		if (Config::RENDER_CUBES_WITH_PLANE)
		{
			Cube* marcosCube = new Cube("Marco's Cube 1", shader_byte_code, size_shader);
			Cube* marcosCube2 = new Cube("Marco's Cube 2", shader_byte_code, size_shader);
			Cube* marcosCube3 = new Cube("Marco's Cube 3", shader_byte_code, size_shader);
			Plane* marcosPlane = new Plane("Marco's Plane", shader_byte_code, size_shader);

			marcosCube->setPosition(Vector3D(0.0f, 0.9f, 0.0f));
			marcosCube2->setPosition(Vector3D(-1.5f, 2.0f, 0.0f));
			marcosCube3->setPosition(Vector3D(-1.5f, 3.0f, -2.0f));
			marcosPlane->setScale(Vector3D(5.0f, 5.0f, 5.0f));
			marcosPlane->setPosition(Vector3D(1.0f));

			this->objectsInWorld.push_back(marcosCube);
			this->objectsInWorld.push_back(marcosCube2);
			this->objectsInWorld.push_back(marcosCube3);
			this->objectsInWorld.push_back(marcosPlane);
		}
		else
		{
			Cube* marcosCube = new Cube("Marco's Cube 1", shader_byte_code, size_shader);
			marcosCube->setPosition(Vector3D(0.0f));

			this->objectsInWorld.push_back(marcosCube);
		}
		if (Config::RENDER_HOUSE_OF_CARDS)
		{
			this->spawnHouseOfCards(shader_byte_code, size_shader);
		}
	}
	else
	{
		for (int i = 0; i < Config::NUMBER_OF_RANDOMIZED_CUBES; i++)
		{
			Cube* marcosCube = new Cube("Marco's Cube " + i + 1, shader_byte_code, size_shader);

			//Get Randomized x, y and z values
			float x = this->randomizeFromRange(-5.0f, 5.0f);
			float y = this->randomizeFromRange(-5.0f, 5.0f);
			float z = this->randomizeFromRange(-5.0f, 5.0f);

			//Set Randomized Positions
			marcosCube->setPosition(x, y, z);
			//Place in vector
			this->objectsInWorld.push_back(marcosCube);
		}
	}

	this->initializeFirstAsSelected();

}

void AppWindow::spawnHouseOfCards(void* shader_byte_code, size_t size_shader)
{
	const float PI = 3.1415926535f;

	const float cardWidth = 1.0f;
	const float cardHeight = 1.4f;
	const float cardThickness = 0.05f;
	const float leanAngle = 15.0f * PI / 180.0f; // 15 degrees lean

	Vector3D cardScale(cardWidth, cardHeight, cardThickness);

	float layer1_y = (cardHeight / 2.0f) * cos(leanAngle);
	for (int i = 0; i < 3; ++i)
	{
		float x_pos = (i - 1) * (cardWidth + 0.05f);

		Cube* leftCard = new Cube("L1_Left_" + std::to_string(i), shader_byte_code, size_shader);
		leftCard->setScale(cardScale);
		leftCard->setRotation(0.0f, 0.0f, leanAngle);
		leftCard->setPosition(x_pos - (cardWidth / 2.0f) * cos(leanAngle), layer1_y, 0.0f);
		this->objectsInWorld.push_back(leftCard);

		Cube* rightCard = new Cube("L1_Right_" + std::to_string(i), shader_byte_code, size_shader);
		rightCard->setScale(cardScale);
		rightCard->setRotation(0.0f, 0.0f, -leanAngle);
		rightCard->setPosition(x_pos + (cardWidth / 2.0f) * cos(leanAngle), layer1_y, 0.0f);
		this->objectsInWorld.push_back(rightCard);
	}

	for (int i = 0; i < 2; ++i)
	{
		float x_pos = (i - 0.5f) * (cardWidth + 0.05f);
		Cube* horizontalCard = new Cube("L1_Horizontal_" + std::to_string(i), shader_byte_code, size_shader);
		horizontalCard->setScale(cardScale);
		horizontalCard->setRotation(PI / 2.0f, 0.0f, 0.0f);
		horizontalCard->setPosition(x_pos, layer1_y * 2.0f, 0.0f);
		this->objectsInWorld.push_back(horizontalCard);
	}

	float layer2_y = layer1_y * 2.0f + (cardHeight / 2.0f) * cos(leanAngle);
	for (int i = 0; i < 2; ++i)
	{
		float x_pos = (i - 0.5f) * (cardWidth + 0.05f);

		Cube* leftCard = new Cube("L2_Left_" + std::to_string(i), shader_byte_code, size_shader);
		leftCard->setScale(cardScale);
		leftCard->setRotation(0.0f, 0.0f, leanAngle);
		leftCard->setPosition(x_pos - (cardWidth / 2.0f) * cos(leanAngle), layer2_y, 0.0f);
		this->objectsInWorld.push_back(leftCard);

		Cube* rightCard = new Cube("L2_Right_" + std::to_string(i), shader_byte_code, size_shader);
		rightCard->setScale(cardScale);
		rightCard->setRotation(0.0f, 0.0f, -leanAngle);
		rightCard->setPosition(x_pos + (cardWidth / 2.0f) * cos(leanAngle), layer2_y, 0.0f);
		this->objectsInWorld.push_back(rightCard);
	}

	Cube* topHorizontalCard = new Cube("L2_Horizontal", shader_byte_code, size_shader);
	topHorizontalCard->setScale(cardScale);
	topHorizontalCard->setRotation(PI / 2.0f, 0.0f, 0.0f);
	topHorizontalCard->setPosition(0.0f, layer2_y + layer1_y, 0.0f); 
	this->objectsInWorld.push_back(topHorizontalCard);


	float layer3_y = layer2_y + 2.0f * layer1_y; 

	Cube* final_leftCard = new Cube("L3_Left", shader_byte_code, size_shader);
	final_leftCard->setScale(cardScale);
	final_leftCard->setRotation(0.0f, 0.0f, leanAngle);
	final_leftCard->setPosition(0.0f - (cardWidth / 2.0f) * cos(leanAngle), layer3_y, 0.0f);
	this->objectsInWorld.push_back(final_leftCard);

	Cube* final_rightCard = new Cube("L3_Right", shader_byte_code, size_shader);
	final_rightCard->setScale(cardScale);
	final_rightCard->setRotation(0.0f, 0.0f, -leanAngle);
	final_rightCard->setPosition(0.0f + (cardWidth / 2.0f) * cos(leanAngle), layer3_y, 0.0f);
	this->objectsInWorld.push_back(final_rightCard);
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

	if (Config::ENABLE_FPS_CAMERA)
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
	for (BaseGameObject* object : this->objectsInWorld)
	{
		delete object;
	}
	this->objectsInWorld.clear();
}

void AppWindow::initializeFirstAsSelected()
{
	if (!this->objectsInWorld.empty())
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