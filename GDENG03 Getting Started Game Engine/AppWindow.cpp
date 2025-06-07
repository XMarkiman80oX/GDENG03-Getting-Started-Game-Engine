#include "AppWindow.h"
#include <Windows.h>
#include "Vector3D.h"
#include "Matrix4x4.h"
#include <time.h> // Required for seeding the random number generator


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
	// Seed the random number generator to ensure different positions/scales each run
	srand((unsigned int)time(NULL));
}

AppWindow::~AppWindow()
{
}

void AppWindow::onCreate()
{
	//Window::onCreate();
	GraphicsEngine::getInstance()->init();
	this->m_swap_chain = GraphicsEngine::getInstance()->createSwapChain();

	RECT rc = this->getClientWindowRect();
	this->m_swap_chain->init(this->m_hwnd, rc.right - rc.left /* Width */, rc.bottom - rc.top /* Height */);

	void* shader_byte_code = nullptr;
	size_t size_shader = 0;
	/*----------------VERTEX SHADER PART----------------*/
	GraphicsEngine::getInstance()->compileVertexShader(L"VertexShader.hlsl", "main", &shader_byte_code, &size_shader);
	this->m_vertex_shader = GraphicsEngine::getInstance()->createVertexShader(shader_byte_code, size_shader);

	// Loop to create and configure each of the 100 cubes
	for (int i = 0; i < cubeCount; i++)
	{
		this->m_cubes[i] = new Cube("MyCube", shader_byte_code, size_shader);

		// Set a random position to scatter the cubes across the screen
		float x = (((float)rand()) / RAND_MAX) * 2.5f - 1.25f;
		float y = (((float)rand()) / RAND_MAX) * 2.0f - 1.0f;
		this->m_cubes[i]->setPosition(x, y, 0.0f);

		// Set a smaller, random scale for each cube
		float scale = (((float)rand()) / RAND_MAX) * 0.05f + 0.02f; // Scale between 0.02 and 0.07
		this->m_cubes[i]->setScale(scale, scale, scale);

		// Set a random rotation speed for each cube
		float rot_x_speed = m_min_rot_speed + static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / (m_max_rot_speed - m_min_rot_speed)));
		float rot_y_speed = m_min_rot_speed + static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / (m_max_rot_speed - m_min_rot_speed)));
		float rot_z_speed = m_min_rot_speed + static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / (m_max_rot_speed - m_min_rot_speed)));
		this->m_cubes[i]->setRotationSpeed(Vector3D(rot_x_speed, rot_y_speed, rot_z_speed));
	}


	GraphicsEngine::getInstance()->releaseCompiledShader();
	/*------------------------------------------------*/

	/*----------------PIXEL SHADER PART----------------*/
	GraphicsEngine::getInstance()->compilePixelShader(L"PixelShader.hlsl", "main", &shader_byte_code, &size_shader);
	this->m_pixel_shader = GraphicsEngine::getInstance()->createPixelShader(shader_byte_code, size_shader);
	GraphicsEngine::getInstance()->releaseCompiledShader();
	/*------------------------------------------------*/
}

void AppWindow::onUpdate()
{
	//Window::onUpdate();
	//change color here
	GraphicsEngine::getInstance()->getImmediateDeviceContext()->clearRenderTargetColor(this->m_swap_chain,
		0, 0.3f, 0.4f, 1);
	RECT rc = this->getClientWindowRect();
	GraphicsEngine::getInstance()->getImmediateDeviceContext()->setViewportSize(rc.right - rc.left, rc.bottom - rc.top);

	this->update();

	GraphicsEngine::getInstance()->getImmediateDeviceContext()->setVertexShader(this->m_vertex_shader);
	GraphicsEngine::getInstance()->getImmediateDeviceContext()->setPixelShader(this->m_pixel_shader);

	// Loop to draw all 100 cubes
	for (int i = 0; i < cubeCount; i++)
	{
		this->m_cubes[i]->draw(rc.right - rc.left, rc.bottom - rc.top, this->m_vertex_shader, this->m_pixel_shader);
	}


	m_swap_chain->present(true);

	this->m_old_delta = this->m_new_delta;
	this->m_new_delta = ::GetTickCount();

	//if the old delta has no value, set it to 0 so we dont have a new delta that equals to the new delta one
	this->m_delta_time = (this->m_old_delta) ? ((this->m_new_delta - this->m_old_delta) / 1000.0f) : 0;
}

void AppWindow::onDestroy()
{
	Window::onDestroy();
	this->m_swap_chain->release();
	this->m_vertex_shader->release();
	this->m_pixel_shader->release();

	// Loop to release the memory for all 100 cubes
	for (int i = 0; i < cubeCount; i++)
	{
		delete this->m_cubes[i];
	}

	GraphicsEngine::getInstance()->release();
}

void AppWindow::update()
{
	// Loop to update the rotation of all 100 cubes
	for (int i = 0; i < cubeCount; i++)
	{
		this->m_cubes[i]->update(this->m_delta_time);
	}
}