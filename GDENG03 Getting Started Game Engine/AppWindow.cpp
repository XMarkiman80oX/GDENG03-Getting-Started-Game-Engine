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
    this->duration = 1000.0f; // Duration before randomizing positions
}

AppWindow::~AppWindow()
{
    // CircleManager::get()->releaseCircles(); // Ensure cleanup if not done elsewhere,
                                            // but CircleManager's destructor should handle it.
}

void AppWindow::onCreate()
{
    this->start_time = GetTickCount();
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
    int numberOfCirclesToSpawn = 10; 
    float lowerBoundRadii = 0.25f;
    float upperBoundRadiiIncrement = 0.5f;
    std::random_device rd; 
    std::mt19937 eng(rd()); 
    std::uniform_real_distribution<> distrPos(-0.8f, 0.8f); // Position range
    // MODIFIED LINE FOR RADIUS:
    std::uniform_real_distribution<> distrRadius(lowerBoundRadii, lowerBoundRadii + upperBoundRadiiIncrement); 
    std::uniform_real_distribution<> distrColor(0.1f, 1.0f);  // Color component range

    for (int i = 0; i < numberOfCirclesToSpawn; i++) 
    {
        vec3 randomPosition( 
            static_cast<float>(distrPos(eng)), 
            static_cast<float>(distrPos(eng)), 
            0.0f 
        );
        float randomRadius = static_cast<float>(distrRadius(eng)); 
        vec3 randomColor( 
            static_cast<float>(distrColor(eng)), 
            static_cast<float>(distrColor(eng)), 
            static_cast<float>(distrColor(eng)) 
        );
        CircleManager::get()->spawnCircle(randomPosition, randomColor, this->aspectRatio, randomRadius); //
    }

    this->loadBuffersAndShaders(); 
}

void AppWindow::onUpdate()
{
    RECT rc = this->getClientWindowRect(); 
    UINT newScreenWidth = rc.right - rc.left;
    UINT newScreenHeight = rc.bottom - rc.top;

    // Ensure no division by zero if window is minimized
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

        // Update aspect ratio for all existing circles and trigger regeneration
        for (Circle* circle : CircleManager::get()->getCircles()) // Assumes getCircles() is public
        {
            circle->setScreenAspectRatio(this->aspectRatio); // New method in Circle
        }

        // After updating all circles, CircleManager needs to rebuild its master vertex list
        CircleManager::get()->rebuildAllCircleVertices(); // New method in CircleManager

        // Reload buffers with the newly updated vertex data
        this->loadBuffersAndShaders(); 
    }

    GraphicsEngine::get()->getImmediateDeviceContext()->clearRenderTargetColor(this->m_swap_chain, 0.0f, 0.0f, 0.0f, 1); //
    // Use the current screen dimensions for the viewport
    GraphicsEngine::get()->getImmediateDeviceContext()->setViewportSize(newScreenWidth, newScreenHeight); 

    ID3D11RasterizerState* noCullState = GraphicsEngine::get()->getSolidNoCullRasterizerState(); 
    if (noCullState) 
    {
        GraphicsEngine::get()->getImmediateDeviceContext()->getDeviceContext()->RSSetState(noCullState); 
    }

    DWORD current_time = GetTickCount(); 
    float elapsed_time = static_cast<float>(current_time - this->start_time); 

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
    if (this->keys[0x4D] && this->isPressed) // M key for MSAA
    {
        // ... (MSAA toggle logic remains the same) ...
        std::cout << "M KEY PRESSED - Toggling MSAA" << std::endl;
        RECT rc_msaa = this->getClientWindowRect(); // Renamed to avoid conflict
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

        std::random_device rd_space; // Use a different name for rd if preferred, or reuse if scope is fine
        std::mt19937 eng_space(rd_space());
        // MODIFIED LINE FOR RADIUS:
        std::uniform_real_distribution<> distrRadius_space(0.10f, 0.20f); // Old range was (0.05f, 0.15f)
        float newRandomRadius = static_cast<float>(distrRadius_space(eng_space)); 

        std::uniform_real_distribution<> distrColor_space(0.1f, 1.0f);
        vec3 randomColor
        ( 
            static_cast<float>(distrColor_space(eng_space)), 
            static_cast<float>(distrColor_space(eng_space)), 
            static_cast<float>(distrColor_space(eng_space)) 
        );

        CircleManager::get()->spawnCircle(randomizedPosition, randomColor, this->aspectRatio, newRandomRadius); 
        this->loadBuffersAndShaders(); // Reload all buffers //
        this->isPressed = false; 
    }
    if (this->keys[VK_DELETE] && this->isPressed)
    {
        std::cout << "DELETE PRESSED - Releasing all circles" << std::endl;
        CircleManager::get()->releaseCircles();
        this->loadBuffersAndShaders(); // Reload (likely empty) buffers
        this->isPressed = false;
    }
    if (this->keys[VK_ESCAPE] && this->isPressed)
    {
        std::cout << "ESCAPE PRESSED - Quitting" << std::endl;
        this->isPressed = false;
        this->onDestroy(); // This calls PostQuitMessage via Window::onDestroy
    }
    if (this->keys[VK_BACK] && this->isPressed)
    {
        std::cout << "BACKSPACE PRESSED - Popping one circle" << std::endl;
        CircleManager::get()->popCircle();
        this->loadBuffersAndShaders(); // Reload buffers
        this->isPressed = false;
    }

    // --- Timed update for movement randomization ---
    //if (elapsed_time >= this->duration)
    //{
    //    std::cout << "Duration reached. Randomizing new positions for circles." << std::endl;
    //    this->start_time = GetTickCount(); // Reset the timer for the next cycle
    //    CircleManager::get()->randomizeNewPositions(); // This now updates targets and regenerates all vertex data in CircleManager
    //    this->loadBuffersAndShaders(); // Reload the vertex buffer with new data
    //}
}

void AppWindow::onDestroy()
{
    Window::onDestroy(); // This sets m_is_run = false and calls PostQuitMessage

    // Release AppWindow specific D3D resources
    if (this->m_constant_buffer) this->m_constant_buffer->release();
    if (this->m_vertex_buffer) this->m_vertex_buffer->release(); // VertexBuffer::release also deletes 'this'

    if (this->m_swap_chain)
    {
        this->m_swap_chain->release();
        delete this->m_swap_chain; // SwapChain new/delete is managed by AppWindow
        this->m_swap_chain = nullptr;
    }

    if (this->m_vertex_shader) this->m_vertex_shader->release(); // VertexShader::release also deletes 'this'
    if (this->m_pixel_shader) this->m_pixel_shader->release();  // PixelShader::release also deletes 'this'

    // CircleManager is a singleton, its cleanup (deleting Circle objects) happens in its destructor
    // or when releaseCircles is called. If AppWindow is the sole controller, ensure releaseCircles is called.
    CircleManager::get()->releaseCircles(); // Explicitly release circles here too for safety.

    GraphicsEngine::get()->release(); // GraphicsEngine is a singleton, release its resources
}

// SpawnCircle method is removed as its logic is now directly in onCreate or input handlers.
// void AppWindow::SpawnCircle(vec3 position, vec3 color, bool isFirstTime) { ... }

void AppWindow::loadBuffersAndShaders()
{
    // This function now correctly reloads based on CircleManager's complete list of vertices
    UINT size_list = CircleManager::get()->getAllCircleVertices().size();
    void* shader_byte_code = nullptr;
    size_t size_shader = 0;

    // Release old buffers if they exist, to prevent leaks if this is called multiple times
    if (this->m_vertex_buffer) {
        // VertexBuffer::release() deletes the object, so we need to nullify and re-create
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
    if (size_list > 0) // Only load if there are vertices
    {
        // Compile and Create Vertex Shader (only needs to be done once, but for simplicity here)
        GraphicsEngine::get()->compileVertexShader(L"VertexShader.hlsl", "main", &shader_byte_code, &size_shader);
        this->m_vertex_shader = GraphicsEngine::get()->createVertexShader(shader_byte_code, size_shader);

        // Load vertex data into the buffer
        this->m_vertex_buffer->load(static_cast<void*>(CircleManager::get()->getAllCircleVertices().data()), sizeof(newVertex), size_list, shader_byte_code, size_shader);
        GraphicsEngine::get()->releaseCompiledShader(); // Release blob from vertex shader compilation
    }
    else { // Handle case of no vertices (e.g., after DELETE and before new spawn)
        // Compile and Create Vertex Shader even if no vertices, so m_vertex_shader is not null for pipeline
        GraphicsEngine::get()->compileVertexShader(L"VertexShader.hlsl", "main", &shader_byte_code, &size_shader);
        this->m_vertex_shader = GraphicsEngine::get()->createVertexShader(shader_byte_code, size_shader);
        GraphicsEngine::get()->releaseCompiledShader();
        // Load an empty buffer essentially, or ensure shaders can handle 0 vertices.
        // The load function for VertexBuffer should handle size_list = 0 gracefully (e.g., not creating D3D buffer or creating a tiny one).
        // The draw call will then draw 0 vertices.
        this->m_vertex_buffer->load(nullptr, sizeof(newVertex), 0, shader_byte_code, size_shader);
    }


    // Compile and Create Pixel Shader
    GraphicsEngine::get()->compilePixelShader(L"PixelShader.hlsl", "main", &shader_byte_code, &size_shader);
    this->m_pixel_shader = GraphicsEngine::get()->createPixelShader(shader_byte_code, size_shader);
    GraphicsEngine::get()->releaseCompiledShader(); // Release blob from pixel shader compilation

    // Create Constant Buffer
    constant cc_init;
    cc_init.m_time = 0;
    cc_init.m_duration = this->duration;
    this->m_constant_buffer = GraphicsEngine::get()->createConstantBuffer();
    this->m_constant_buffer->load(&cc_init, sizeof(constant));

    std::cout << "Buffers and shaders reloaded. Vertex count: " << size_list << std::endl;
}
