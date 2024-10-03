#include "AppWindow.h"
#include <Windows.h>
#include <wincodec.h>
#include <iostream>
#include <DirectXMath.h>

struct vec3
{
	float x, y, z;
};

struct vertex 
{
	DirectX::XMFLOAT3 position1; // First position
	DirectX::XMFLOAT3 position2; // Second position
	DirectX::XMFLOAT3 color1;    // First color
	DirectX::XMFLOAT3 color2;    // Second color
	DirectX::XMFLOAT2 texCoord;   // Texture coordinates
};
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
	// Initialize COM
	HRESULT hr = CoInitialize(nullptr);
	if (FAILED(hr)) {
		std::cerr << "Failed to initialize COM" << std::endl;
		return; // Handle error appropriately
	}
	//Window::onCreate();
	GraphicsEngine::get()->init();
	this->m_swap_chain = GraphicsEngine::get()->createSwapChain();

	this->initializeWIC();

	RECT rc = this->getClientWindowRect();
	this->m_swap_chain->init(this->m_hwnd, rc.right - rc.left, rc.bottom - rc.top);

	// Initialize the vertex list using the Vertex struct
	vertex list[] = {
		{ { -0.5f, -0.5f, 0.0f }, { -0.32f, -0.11f, 0.0f }, { 0, 1, 0 }, { 0, 1, 0 }, { 0, 0 } }, // POS1
		{ { -0.5f,  0.5f, 0.0f }, { -0.11f,  0.78f, 0.0f }, { 0, 1, 0 }, { 0, 1, 0 }, { 0, 1 } }, // POS2
		{ {  0.5f, -0.5f, 0.0f }, {  0.75f, -0.73f, 0.0f }, { 0, 1, 0 }, { 1, 0, 0 }, { 1, 0 } }, // POS3
		{ {  0.5f,  0.5f, 0.0f }, {  0.88f,  0.77f, 0.0f }, { 0, 1, 0 }, { 0, 0, 1 }, { 1, 1 } }, // POS4
	};
	this->m_vb = GraphicsEngine::get()->createVertexBuffer();
	UINT size_list = ARRAYSIZE(list);

	void* shader_byte_code = nullptr;
	size_t size_shader = 0;
	GraphicsEngine::get()->compileVertexShader(L"VertexShader.hlsl", "main", &shader_byte_code, &size_shader);
	
	this->m_vs = GraphicsEngine::get()->createVertexShader(shader_byte_code, size_shader);
	
	this->m_vb->load(list, sizeof(vertex), size_list, shader_byte_code, size_shader);

	GraphicsEngine::get()->releaseCompiledShader();

	GraphicsEngine::get()->compilePixelShader(L"PixelShader.hlsl", "main", &shader_byte_code, &size_shader);

	this->m_ps = GraphicsEngine::get()->createPixelShader(shader_byte_code, size_shader);

	GraphicsEngine::get()->releaseCompiledShader();

	constant cc;
	cc.m_time = 0;

	this->m_cb = GraphicsEngine::get()->createConstantBuffer();
	this->m_cb->load(&cc, sizeof(constant));

	m_texture = GraphicsEngine::get()->createTexture();

	if (!m_texture->loadFromFile(L"C:\\Users\\Asus.DESKTOP-59PAOPP\\source\\repos\\GDENG03 Getting Started Game Engine\\GDENG03 Getting Started Game Engine\\Textures\\MyTexture.png")) {
		// Handle texture loading error
		std::cout << "Texture Not Found In Load From File" << std::endl;
	}
}
void AppWindow::onUpdate()
{
	//Window::onUpdate();
	//change color here
	GraphicsEngine::get()->getImmediateDeviceContext()->clearRenderTargetColor(this->m_swap_chain,
		0,0.3f,0.4f,1);
	RECT rc = this->getClientWindowRect();
	GraphicsEngine::get()->getImmediateDeviceContext()->setViewportSize(rc.right - rc.left, rc.bottom - rc.top);
	
	constant cc;
	cc.m_time = GetTickCount();
	this->m_cb->update(GraphicsEngine::get()->getImmediateDeviceContext(), &cc);
	GraphicsEngine::get()->getImmediateDeviceContext()->setConstantBuffer(m_vs, m_cb);
	GraphicsEngine::get()->getImmediateDeviceContext()->setConstantBuffer(m_ps, m_cb);

	GraphicsEngine::get()->getImmediateDeviceContext()->setVertexShader(this->m_vs);
	GraphicsEngine::get()->getImmediateDeviceContext()->setPixelShader(this->m_ps);

	GraphicsEngine::get()->getImmediateDeviceContext()->setVertexBuffer(m_vb); 
	
	ID3D11DeviceContext* dxContext = GraphicsEngine::get()->getImmediateDeviceContext()->getDeviceContext();

	// Bind the texture
	m_texture->bind(dxContext);

	GraphicsEngine::get()->getImmediateDeviceContext()->drawTriangleStrip(m_vb->getSizeVertexList(), 0);
	m_swap_chain->present(true);
}

void AppWindow::onDestroy()
{
	Window::onDestroy();
	this->m_vb->release();
	this->m_swap_chain->release();

	this->m_vs->release();
	this->m_ps->release();
	GraphicsEngine::get()->release();

	if (m_texture) {
		m_texture->release();
		delete m_texture;
	}
	CoUninitialize(); // Clean up COM
}

void AppWindow::initializeWIC()
{
	// Initialize WIC
	IWICImagingFactory* pFactory = nullptr;
	HRESULT hr = CoCreateInstance(
		CLSID_WICImagingFactory,
		NULL,
		CLSCTX_INPROC_SERVER,
		IID_PPV_ARGS(&pFactory)
	);
	if (FAILED(hr)) {
		std::cerr << "Failed to create WIC Imaging Factory" << std::endl;
		return; // Handle error appropriately
	}
}
