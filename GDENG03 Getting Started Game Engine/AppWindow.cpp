#include "AppWindow.h"
#include <Windows.h>

__declspec(align(16))
struct constant 
{
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
	//Window::onCreate();
	GraphicsEngine::get()->init();
	this->m_swap_chain = GraphicsEngine::get()->createSwapChain();

	RECT rc = this->getClientWindowRect();
	UINT screenWidth = rc.right - rc.left;
	UINT screenHeight = rc.bottom - rc.top;
	this->m_swap_chain->init(this->m_hwnd, screenWidth, screenHeight);

	float aspectRatio = (float)screenWidth / screenHeight;

	CircleManager::get()->spawnCircle(vec3(-0.5f, 0, 0), vec3(1, 1, 1), aspectRatio);
	CircleManager::get()->spawnCircle(vec3(0.0f, 0, 0), vec3(1, 1, 1), aspectRatio);

		//Here we create the vertex buffer, then the established vertex list will be loaded here later on
	this->m_vertex_buffer = GraphicsEngine::get()->createVertexBuffer();

	UINT size_list = CircleManager::get()->getAllCircleVertices().size();

	std::cout << "Size List: " <<size_list<< std::endl;
	void* shader_byte_code = nullptr;
	size_t size_shader = 0;


	GraphicsEngine::get()->compileVertexShader(L"VertexShader.hlsl", "main", &shader_byte_code, &size_shader);

	this->m_vertex_shader = GraphicsEngine::get()->createVertexShader(shader_byte_code, size_shader);

	this->m_vertex_buffer->load(static_cast<void*>(CircleManager::get()->getAllCircleVertices().data()), sizeof(newVertex), size_list, shader_byte_code, size_shader);

	GraphicsEngine::get()->releaseCompiledShader();


	GraphicsEngine::get()->compilePixelShader(L"PixelShader.hlsl", "main", &shader_byte_code, &size_shader);

	this->m_pixel_shader = GraphicsEngine::get()->createPixelShader(shader_byte_code, size_shader);

	GraphicsEngine::get()->releaseCompiledShader();

	constant cc;
	cc.m_time = 0;

	this->m_cb = GraphicsEngine::get()->createConstantBuffer();
	this->m_cb->load(&cc, sizeof(constant));

}

void AppWindow::onUpdate()
{
	//Window::onUpdate();
	//change color here
	GraphicsEngine::get()->getImmediateDeviceContext()->clearRenderTargetColor(this->m_swap_chain,
		0.0f,0.0f,0.0f,1);
	RECT rc = this->getClientWindowRect();
	GraphicsEngine::get()->getImmediateDeviceContext()->setViewportSize(rc.right - rc.left, rc.bottom - rc.top);
	
	constant cc;
	cc.m_time = GetTickCount();
	//std::cout << cc.m_time << std::endl;
	this->m_cb->update(GraphicsEngine::get()->getImmediateDeviceContext(), &cc);
	GraphicsEngine::get()->getImmediateDeviceContext()->setConstantBuffer(m_vertex_shader, m_cb);
	GraphicsEngine::get()->getImmediateDeviceContext()->setConstantBuffer(m_pixel_shader, m_cb);

	GraphicsEngine::get()->getImmediateDeviceContext()->setVertexShader(this->m_vertex_shader);
	GraphicsEngine::get()->getImmediateDeviceContext()->setPixelShader(this->m_pixel_shader);

	GraphicsEngine::get()->getImmediateDeviceContext()->setVertexBuffer(m_vertex_buffer);

	GraphicsEngine::get()->getImmediateDeviceContext()->drawTriangleStrip(m_vertex_buffer->getSizeVertexList(), 1);
	//std::cout << m_vertex_buffer->getSizeVertexList() << std::endl;
	//GraphicsEngine::get()->getImmediateDeviceContext()->drawCircles(CircleManager::get()->getCircleCount(), m_vertex_buffer->getSizeVertexList(), CircleManager::get()->getDefaultSegmentCount() + 2);
	m_swap_chain->present(true);
}

void AppWindow::onDestroy()
{
	Window::onDestroy();
	this->m_vertex_buffer->release();
	this->m_swap_chain->release();

	this->m_vertex_shader->release();
	this->m_pixel_shader->release();
	GraphicsEngine::get()->release();
}

void AppWindow::DrawTriangle()
{
}
