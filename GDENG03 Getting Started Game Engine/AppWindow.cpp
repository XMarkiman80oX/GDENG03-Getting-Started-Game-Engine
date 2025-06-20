#include "AppWindow.h"
#include "EngineTime.h"
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
   /* this->start_time = GetTickCount();
    this->last_time = GetTickCount();*/
    GraphicsEngine::get()->init();

    this->m_swap_chain = GraphicsEngine::get()->createSwapChain();

    RECT rc = this->getClientWindowRect();
    UINT screenWidth = rc.right - rc.left;
    UINT screenHeight = rc.bottom - rc.top;

    if (!this->m_swap_chain->init(this->m_hwnd, screenWidth, screenHeight,
        GraphicsEngine::get()->getMSAASampleCount(),
        GraphicsEngine::get()->getMSAAQualityLevels()))
    {
        std::cout << "Failed to initialize SwapChain in AppWindow::onCreate!" << std::endl;
        PostQuitMessage(0);
        return;
    }

    this->aspectRatio = (float)screenWidth / screenHeight;

    // --- Spawning multiple circles ---
    int numberOfCirclesToSpawn = 4; 
    float lowerBoundRadii = 0.125f;
    float upperBoundRadiiIncrement = 0.25f;

    for (int i = 0; i < numberOfCirclesToSpawn; i++) 
    {
        float randomX = this->randomizeFromRange(-0.8f, 0.8f);
        float randomY = this->randomizeFromRange(-0.8f, 0.8f);

        vec3 randomPosition(randomX,randomY,0.0f);

        float randomRadius = this->randomizeFromRange(lowerBoundRadii, lowerBoundRadii + upperBoundRadiiIncrement);

        float r = this->randomizeFromRange(0.1f, 1.0f);
        float g = this->randomizeFromRange(0.1f, 1.0f);
        float b = this->randomizeFromRange(0.1f, 1.0f);

        vec3 randomColor(r,g,b);

        CircleManager::get()->spawnCircle(randomPosition, randomColor, this->aspectRatio, randomRadius); 
    }

    this->loadBuffersAndShaders(); 
}

void AppWindow::onUpdate()
{ 
    float delta_time = static_cast<float> (EngineTime::getDeltaTime());

    //Update positions and handle bounces for all circles
    for (Circle* circle : CircleManager::get()->getCircles())
    {
        circle->update(delta_time);
    }

    // After updating all circle positions, rebuild the master vertex list
    CircleManager::get()->rebuildAllCircleVertices();

    //Reload the vertex buffer with the updated data
    this->loadBuffersAndShaders();
    RECT rc = this->getClientWindowRect(); 
    UINT newScreenWidth = rc.right - rc.left;
    UINT newScreenHeight = rc.bottom - rc.top;

    //Ensure no division by zero if window is minimized
    float newAspectRatio = 1.0f; // Default aspect ratio
    if (newScreenWidth > 0 && newScreenHeight > 0) {
        newAspectRatio = static_cast<float>(newScreenWidth) / newScreenHeight;
    }

    bool aspectRatioChanged = false;

    if (std::abs(this->aspectRatio - newAspectRatio) > 0.001f)
    {
        this->aspectRatio = newAspectRatio; 
        aspectRatioChanged = true;
        std::cout << "Aspect ratio changed to: " << this->aspectRatio << std::endl;

        //Update aspect ratio for all existing circles and trigger regeneration
        for (Circle* circle : CircleManager::get()->getCircles()) // Assumes getCircles() is public
        {
            circle->setScreenAspectRatio(this->aspectRatio); // New method in Circle
        }

        //After updating all circles, CircleManager needs to rebuild its master vertex list
        CircleManager::get()->rebuildAllCircleVertices(); // New method in CircleManager

        //Reload buffers with the newly updated vertex data
        this->loadBuffersAndShaders(); 
    }

    GraphicsEngine::get()->getImmediateDeviceContext()->clearRenderTargetColor(this->m_swap_chain, 0.0f, 0.0f, 0.0f, 1); 
    GraphicsEngine::get()->getImmediateDeviceContext()->setViewportSize(newScreenWidth, newScreenHeight); 

    ID3D11RasterizerState* noCullState = GraphicsEngine::get()->getSolidNoCullRasterizerState(); 
    if (noCullState) 
    {
        GraphicsEngine::get()->getImmediateDeviceContext()->getDeviceContext()->RSSetState(noCullState); 
    }

    float elapsed_time = static_cast<float> (EngineTime::getTotalElapsedTime());

    constant cc;
    cc.m_time = static_cast<unsigned int>(elapsed_time); 
    cc.m_duration = this->duration; 

    this->m_constant_buffer->update(GraphicsEngine::get()->getImmediateDeviceContext(), &cc); 

    GraphicsEngine::get()->getImmediateDeviceContext()->setConstantBuffer(m_vertex_shader, m_constant_buffer); 
    GraphicsEngine::get()->getImmediateDeviceContext()->setConstantBuffer(m_pixel_shader, m_constant_buffer); 

    GraphicsEngine::get()->getImmediateDeviceContext()->setVertexShader(this->m_vertex_shader); 
    GraphicsEngine::get()->getImmediateDeviceContext()->setPixelShader(this->m_pixel_shader); 

    GraphicsEngine::get()->getImmediateDeviceContext()->setVertexBuffer(m_vertex_buffer); 
    GraphicsEngine::get()->getImmediateDeviceContext()->drawTriangleList(m_vertex_buffer->getSizeVertexList(), 0); 

    m_swap_chain->present(true); 

    // --- Input Handling ---
    if (this->keys['M'] && this->isPressed) 
    {
        std::cout << "M KEY PRESSED - Toggling MSAA" << std::endl;
        RECT rc_msaa = this->getClientWindowRect();
        UINT screenWidth_msaa = rc_msaa.right - rc_msaa.left;
        UINT screenHeight_msaa = rc_msaa.bottom - rc_msaa.top;
        GraphicsEngine::get()->toggleMSAA(this->m_hwnd, screenWidth_msaa, screenHeight_msaa);
        if (!GraphicsEngine::get()->reinitializeSwapChain(this->m_hwnd, screenWidth_msaa, screenHeight_msaa, this->m_swap_chain))
        {
            std::cout << "Failed to reinitialize swap chain for MSAA toggle." << std::endl;
        }
        this->isPressed = false;
    }
    if (this->keys[VK_SPACE] && this->isPressed) 
    {
        std::cout << "SPACE PRESSED - Spawning new circle" << std::endl; 
        vec3 randomizedPosition; 
        randomizedPosition.randomizeVector(false, -0.8f, 0.8f); // Confine to screen 

        float newRandomRadius = this->randomizeFromRange(0.1f, 0.2f); 

        float r = this->randomizeFromRange(0.1f, 1.0f);
        float g = this->randomizeFromRange(0.1f, 1.0f);
        float b = this->randomizeFromRange(0.1f, 1.0f);

        vec3 randomColor(r,g,b);

        CircleManager::get()->spawnCircle(randomizedPosition, randomColor, this->aspectRatio, newRandomRadius); 
        this->loadBuffersAndShaders(); 
        this->isPressed = false; 
    }
    if (this->keys[VK_DELETE] && this->isPressed)
    {
        std::cout << "DELETE PRESSED - Releasing all circles" << std::endl;
        CircleManager::get()->releaseCircles();
        this->loadBuffersAndShaders(); 
        this->isPressed = false;
    }
    if (this->keys[VK_ESCAPE] && this->isPressed)
    {
        std::cout << "ESCAPE PRESSED - Quitting" << std::endl;
        this->isPressed = false;
        this->onDestroy(); 
    }
    if (this->keys[VK_BACK] && this->isPressed)
    {
        std::cout << "BACKSPACE PRESSED - Popping one circle" << std::endl;
        CircleManager::get()->popCircle();
        this->loadBuffersAndShaders(); 
        this->isPressed = false;
    }
}

void AppWindow::onDestroy()
{
    Window::onDestroy(); 

    // Release AppWindow specific D3D resources
    if (this->m_constant_buffer) this->m_constant_buffer->release();
    if (this->m_vertex_buffer) this->m_vertex_buffer->release(); 

    if (this->m_swap_chain)
    {
        this->m_swap_chain->release();
        delete this->m_swap_chain; // SwapChain new/delete is managed by AppWindow
        this->m_swap_chain = nullptr;
    }

    if (this->m_vertex_shader) this->m_vertex_shader->release(); 
    if (this->m_pixel_shader) this->m_pixel_shader->release();  

    CircleManager::get()->releaseCircles(); 

    GraphicsEngine::get()->release(); 
}

void AppWindow::loadBuffersAndShaders()
{
    //This function now correctly reloads based on CircleManager's complete list of vertices
    UINT size_list = CircleManager::get()->getAllCircleVertices().size();
    void* shader_byte_code = nullptr;
    size_t size_shader = 0;

    //Release old buffers if they exist, to prevent leaks if this is called multiple times
    if (this->m_vertex_buffer) {
        this->m_vertex_buffer->release();
        this->m_vertex_buffer = nullptr;
    }
    if (this->m_constant_buffer) {
        this->m_constant_buffer->release();
        this->m_constant_buffer = nullptr;
    }
    if (this->m_vertex_shader) {
        this->m_vertex_shader->release();
        this->m_vertex_shader = nullptr;
    }
    if (this->m_pixel_shader) {
        this->m_pixel_shader->release();
        this->m_pixel_shader = nullptr;
    }

    // Create Vertex Buffer
    this->m_vertex_buffer = GraphicsEngine::get()->createVertexBuffer();
    if (size_list > 0)
    {
        GraphicsEngine::get()->compileVertexShader(L"VertexShader.hlsl", "main", &shader_byte_code, &size_shader);
        this->m_vertex_shader = GraphicsEngine::get()->createVertexShader(shader_byte_code, size_shader);

        this->m_vertex_buffer->load(static_cast<void*>(CircleManager::get()->getAllCircleVertices().data()), sizeof(newVertex), size_list, shader_byte_code, size_shader);
        GraphicsEngine::get()->releaseCompiledShader(); // Release blob from vertex shader compilation
    }
    else { 
        GraphicsEngine::get()->compileVertexShader(L"VertexShader.hlsl", "main", &shader_byte_code, &size_shader);
        this->m_vertex_shader = GraphicsEngine::get()->createVertexShader(shader_byte_code, size_shader);
        GraphicsEngine::get()->releaseCompiledShader();
        this->m_vertex_buffer->load(nullptr, sizeof(newVertex), 0, shader_byte_code, size_shader);
    }


    GraphicsEngine::get()->compilePixelShader(L"PixelShader.hlsl", "main", &shader_byte_code, &size_shader);
    this->m_pixel_shader = GraphicsEngine::get()->createPixelShader(shader_byte_code, size_shader);
    GraphicsEngine::get()->releaseCompiledShader();

    constant cc_init;
    cc_init.m_time = 0;
    cc_init.m_duration = this->duration;
    this->m_constant_buffer = GraphicsEngine::get()->createConstantBuffer();
    this->m_constant_buffer->load(&cc_init, sizeof(constant));

}

float AppWindow::randomizeFromRange(float lowerBound, float upperBound)
{
    std::random_device rd;
    std::mt19937 eng(rd());
    std::uniform_real_distribution<> randomizedFloat(lowerBound, upperBound);

    return static_cast<float> (randomizedFloat(eng));
}
