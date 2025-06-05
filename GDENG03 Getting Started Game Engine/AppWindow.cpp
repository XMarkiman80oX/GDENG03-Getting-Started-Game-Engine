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
	GraphicsEngine::get()->init(); // GraphicsEngine::init() now checks MSAA

	this->m_swap_chain = GraphicsEngine::get()->createSwapChain();

	RECT rc = this->getClientWindowRect();
	UINT screenWidth = rc.right - rc.left;
	UINT screenHeight = rc.bottom - rc.top;

	// Initialize SwapChain with current MSAA settings from GraphicsEngine
	if (!this->m_swap_chain->init(this->m_hwnd, screenWidth, screenHeight,
		GraphicsEngine::get()->getMSAASampleCount(),
		GraphicsEngine::get()->getMSAAQualityLevels()))
	{
		// Handle error - perhaps throw an exception or log and exit
		std::cout << "Failed to initialize SwapChain in AppWindow::onCreate!" << std::endl;
		PostQuitMessage(0); // Example: quit the application
		return;
	}


	this->aspectRatio = (float)screenWidth / screenHeight;
	this->SpawnCircle(vec3(-0.5f, 0, 0), vec3(1, 1, 1), true);

}

void AppWindow::onUpdate()
{
	//Window::onUpdate();
	//change color here
	GraphicsEngine::get()->getImmediateDeviceContext()->clearRenderTargetColor(this->m_swap_chain,
		0.0f, 0.0f, 0.0f, 1);
	RECT rc = this->getClientWindowRect();
	GraphicsEngine::get()->getImmediateDeviceContext()->setViewportSize(rc.right - rc.left, rc.bottom - rc.top);
	
	// Set the custom rasterizer state (no culling)
	ID3D11RasterizerState* noCullState = GraphicsEngine::get()->getSolidNoCullRasterizerState();
	if (noCullState) // Ensure it was created
	{
		// If you added setRasterizerState to your DeviceContext class:
		// GraphicsEngine::get()->getImmediateDeviceContext()->setRasterizerState(noCullState);
		// Otherwise, you can call it directly on the raw context if you had access, but the method is cleaner:
		GraphicsEngine::get()->getImmediateDeviceContext()->getDeviceContext()->RSSetState(noCullState); // Accessing m_device_context directly assuming it's public or via a getter
		// Or use the setRasterizerState method if implemented in DeviceContext
	}

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

	GraphicsEngine::get()->getImmediateDeviceContext()->drawTriangleStrip(m_vertex_buffer->getSizeVertexList(), 0); // Use 0 for start_vertex_index
	m_swap_chain->present(true);

	if (this->keys[0x4D] && this->isPressed)
	{
		std::cout << "M KEY PRESSED - Toggling MSAA" << std::endl;
		RECT rc = this->getClientWindowRect();
		UINT screenWidth = rc.right - rc.left;
		UINT screenHeight = rc.bottom - rc.top;

		// Toggle MSAA state in GraphicsEngine
		GraphicsEngine::get()->toggleMSAA(this->m_hwnd, screenWidth, screenHeight);

		// Recreate the swap chain with new MSAA settings
		if (!GraphicsEngine::get()->reinitializeSwapChain(this->m_hwnd, screenWidth, screenHeight, this->m_swap_chain))
		{
			std::cout << "Failed to reinitialize swap chain for MSAA toggle." << std::endl;
			// Potentially handle this error, e.g., by trying to revert or exiting
		}

		this->isPressed = false; // Prevent continuous toggling
	}
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
		CircleManager::get()->randomizeNewPositions();
		this->loadBuffersAndShaders();
	}
}

void AppWindow::onDestroy()
{
	Window::onDestroy();
	// Release AppWindow specific resources
	if (this->m_constant_buffer) this->m_constant_buffer->release();
	if (this->m_vertex_buffer) this->m_vertex_buffer->release();

	// SwapChain is released here
	if (this->m_swap_chain)
	{
		this->m_swap_chain->release(); // This should release its RTV
		delete this->m_swap_chain; // Delete the object itself
		this->m_swap_chain = nullptr;
	}


	if (this->m_vertex_shader) this->m_vertex_shader->release();
	if (this->m_pixel_shader) this->m_pixel_shader->release();

	// GraphicsEngine is a singleton, usually released at the very end of the application
	// or managed differently. If AppWindow is responsible for its lifecycle:
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
