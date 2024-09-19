#include "AppWindow.h"

struct vec3
{
	float x, y, z;
};

struct Vertex 
{
	vec3 position;
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
	this->m_swap_chain->init(this->m_hwnd, rc.right - rc.left, rc.bottom - rc.top);

	Vertex list[] = {
		//X - Y - Z
		{-0.5f, -0.5f, 0.0f}, //POS1
		{0.5f, 0.5f, 0.0f}, //POS2
		{0.5f, -0.5f, 0.0f}, //POS3

		{-0.5f, -0.5f, 0.0f}, //POS1
		{-0.5f, 0.5f, 0.0f}, //POS2
		{0.5f, 0.5f, 0.0f}, //POS3
	};
	this->m_vb = GraphicsEngine::get()->createVertexBuffer();
	UINT size_list = ARRAYSIZE(list);

	GraphicsEngine::get()->createShaders();

	void* shader_byte_code = nullptr;
	UINT size_shader = 0;
	GraphicsEngine::get()->getShaderBufferAndSize(&shader_byte_code, &size_shader);

	this->m_vb->load(list, sizeof(Vertex), size_list, shader_byte_code, size_shader);
}

void AppWindow::onUpdate()
{
	//Window::onUpdate();
	//change color here
	GraphicsEngine::get()->getImmediateDeviceContext()->clearRenderTargetColor(this->m_swap_chain,
		0,0.3f,0.4f,1);
	RECT rc = this->getClientWindowRect();
	GraphicsEngine::get()->getImmediateDeviceContext()->setViewportSize(rc.right - rc.left, rc.bottom - rc.top);
	GraphicsEngine::get()->setShaders();
	GraphicsEngine::get()->getImmediateDeviceContext()->setVertexBuffer(m_vb);

	GraphicsEngine::get()->getImmediateDeviceContext()->drawTriangleList(m_vb->getSizeVertexList(), 0);
	m_swap_chain->present(true);
}

void AppWindow::onDestroy()
{
	Window::onDestroy();
	this->m_vb->release();
	this->m_swap_chain->release();
	GraphicsEngine::get()->release();
}
