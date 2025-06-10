#include "AppWindow.h"
#include <Windows.h>

__declspec(align(16))
struct constant 
{
	unsigned int m_time;
	float m_duration;
};

AppWindow::AppWindow()
{
	this->duration = 1000.0f;
}

AppWindow::~AppWindow()
{
}

void AppWindow::onCreate()
{
	this->start_time = GetTickCount();
	//Window::onCreate();
	GraphicsEngine::get()->init();
	this->m_swap_chain = GraphicsEngine::get()->createSwapChain();

	RECT rc = this->getClientWindowRect();
	UINT screenWidth = rc.right - rc.left;
	UINT screenHeight = rc.bottom - rc.top;
	this->m_swap_chain->init(this->m_hwnd, screenWidth, screenHeight);

	this->aspectRatio = (float)screenWidth / screenHeight;

	//CircleManager::get()->spawnCircle(vec3(-0.5f, 0, 0), vec3(1, 1, 1), aspectRatio);
	this->SpawnCircle(vec3(-0.5f, 0, 0), vec3(1, 1, 1), true);

}

void AppWindow::onUpdate()
{
	//Window::onUpdate();
	//change color here
	GraphicsEngine::get()->getImmediateDeviceContext()->clearRenderTargetColor(this->m_swap_chain,
		0.0f,0.0f,0.0f,1);
	RECT rc = this->getClientWindowRect();
	GraphicsEngine::get()->getImmediateDeviceContext()->setViewportSize(rc.right - rc.left, rc.bottom - rc.top);
	
	DWORD current_time = GetTickCount();
	float elapsed_time = static_cast<float>(current_time - this->start_time);

	constant cc;
	cc.m_time = elapsed_time;
	cc.m_duration = duration;

	this->m_constant_buffer->update(GraphicsEngine::get()->getImmediateDeviceContext(), &cc);
	//std::cout << "m_time: " << cc.m_time << " m_duration: "<< cc.m_duration << std::endl;
	//std::cout << "m_time / m_duration: "<< cc.m_time/cc.m_duration << std::endl;
	//std::cout << cc.m_time << std::endl;
	GraphicsEngine::get()->getImmediateDeviceContext()->setConstantBuffer(m_vertex_shader, m_constant_buffer);
	GraphicsEngine::get()->getImmediateDeviceContext()->setConstantBuffer(m_pixel_shader, m_constant_buffer);

	GraphicsEngine::get()->getImmediateDeviceContext()->setVertexShader(this->m_vertex_shader);
	GraphicsEngine::get()->getImmediateDeviceContext()->setPixelShader(this->m_pixel_shader);

	GraphicsEngine::get()->getImmediateDeviceContext()->setVertexBuffer(m_vertex_buffer);

	GraphicsEngine::get()->getImmediateDeviceContext()->drawTriangleStrip(m_vertex_buffer->getSizeVertexList(), 0);
	m_swap_chain->present(true);
	
	if (this->keys[VK_SPACE] && this->isPressed) 
	{
		std::cout << "SPACE PRESSED" << std::endl;

		vec3 randomizedPosition;
		randomizedPosition.randomizeVector(false, -0.5f, 0.5f);
		std::cout << "New Circle Randomized Position: ";

		this->SpawnCircle(randomizedPosition, vec3(1.0f, 1.0f, 1.0f), false);
		this->isPressed = false;
	}
	if (this->keys[VK_DELETE] && this->isPressed)
	{
		std::cout << "DELETE PRESSED" << std::endl;
		CircleManager::get()->releaseCircles(); 
		this->m_vertex_buffer->load(0, 0, 0, 0, 0);
		this->isPressed = false;
	}
	if (this->keys[VK_ESCAPE] && this->isPressed)
	{
		std::cout << "ESCAPE PRESSED" << std::endl;
		this->isPressed = false;
		this->onDestroy();
	}
	if (this->keys[VK_BACK] && this->isPressed)
	{
		std::cout << "BACKSPACE PRESSED" << std::endl;
		this->isPressed = false;
		CircleManager::get()->popCircle();
		this->loadBuffersAndShaders();
	}
	if (elapsed_time >= this->duration) 
	{
		std::cout << "1 second has passed!" << std::endl;
		CircleManager::get()->randomizeNewPositions();
		this->loadBuffersAndShaders();
	}
}

void AppWindow::onDestroy()
{
	Window::onDestroy();
	this->m_constant_buffer->release();
	this->m_vertex_buffer->release();
	this->m_swap_chain->release();

	this->m_vertex_shader->release();
	this->m_pixel_shader->release();
	GraphicsEngine::get()->release();
}

void AppWindow::SpawnCircle(vec3 position, vec3 color, bool isFirstTime)
{
	CircleManager::get()->spawnCircle(position, color, aspectRatio);

	UINT size_list = CircleManager::get()->getAllCircleVertices().size();
	void* shader_byte_code = nullptr;
	size_t size_shader = 0;

	if (isFirstTime) 
	{
		this->loadBuffersAndShaders();
	}
	else
	{
		this->m_vertex_buffer->load(static_cast<void*>(CircleManager::get()->getAllCircleVertices().data()), sizeof(newVertex), size_list, shader_byte_code, size_shader);
	}
}

void AppWindow::loadBuffersAndShaders()
{
	UINT size_list = CircleManager::get()->getAllCircleVertices().size();
	void* shader_byte_code = nullptr;
	size_t size_shader = 0;

	//Here we create the vertex buffer, then the established vertex list will be loaded here later on
	this->m_vertex_buffer = GraphicsEngine::get()->createVertexBuffer();


	GraphicsEngine::get()->compileVertexShader(L"VertexShader.hlsl", "main", &shader_byte_code, &size_shader);

	this->m_vertex_shader = GraphicsEngine::get()->createVertexShader(shader_byte_code, size_shader);

	this->m_vertex_buffer->load(static_cast<void*>(CircleManager::get()->getAllCircleVertices().data()), sizeof(newVertex), size_list, shader_byte_code, size_shader);

	GraphicsEngine::get()->releaseCompiledShader();


	GraphicsEngine::get()->compilePixelShader(L"PixelShader.hlsl", "main", &shader_byte_code, &size_shader);

	this->m_pixel_shader = GraphicsEngine::get()->createPixelShader(shader_byte_code, size_shader);

	//GraphicsEngine::get()->releaseCompiledShader();

	constant cc;
	cc.m_time = 0;

	this->m_constant_buffer = GraphicsEngine::get()->createConstantBuffer();
	this->m_constant_buffer->load(&cc, sizeof(constant));
}
