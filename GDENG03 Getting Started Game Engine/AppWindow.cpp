#include "AppWindow.h"
#include "Quad.h"
#include "iostream"
#include "vector"
#include "memory"

//struct vec3
//{
//	float x, y, z;
//};

struct vertex
{
	Vector3 position;
	Vector3 color;
};

AppWindow::AppWindow()
{
}


AppWindow::~AppWindow()
{
}

void AppWindow::onCreate()
{
    Window::onCreate();
    GraphicsEngine::get()->init();
    m_swap_chain = GraphicsEngine::get()->createSwapChain();

    RECT rc = this->getClientWindowRect();
    m_swap_chain->init(this->m_hwnd, rc.right - rc.left, rc.bottom - rc.top);

    // Create vector of Quad pointers using smart pointers
    std::vector<std::unique_ptr<Quad>> quads;

    // Create and configure quads
    auto quad1 = std::make_unique<Quad>();
    quad1->setPosition(-0.5f, 0.0f);  // Left
    quads.push_back(std::move(quad1));

    auto quad2 = std::make_unique<Quad>();
    quad2->setPosition(0.0f, 0.0f);    // Center
    quads.push_back(std::move(quad2));

    auto quad3 = std::make_unique<Quad>();
    quad3->setPosition(0.0f, -0.5f);   // Bottom
    quads.push_back(std::move(quad3));

    // Get vertex count from first quad
    const int verticesPerQuad = quads[0]->getVertexCount();
    std::vector<vertex> vertexList;

    // First quad - full vertices
    Vector3* positions = quads[0]->getVertexPositions();
    Vector3* colors = quads[0]->getVertexColors();
    for (int j = 0; j < verticesPerQuad; j++) {
        vertexList.push_back({ positions[j], colors[j] });
        std::cout << positions[j].x << ", " << positions[j].y << std::endl;
    }

    // Subsequent quads with degenerate triangles
    for (int i = 1; i < quads.size(); i++) {
        // Degenerate vertex 1: Last vertex of previous quad
        vertexList.push_back({
            quads[i - 1]->getVertexPositions()[verticesPerQuad - 1],
            quads[i - 1]->getVertexColors()[verticesPerQuad - 1]
            });

        // Degenerate vertex 2: First vertex of current quad
        vertexList.push_back({
            quads[i]->getVertexPositions()[0],
            quads[i]->getVertexColors()[0]
            });

        // Current quad vertices
        positions = quads[i]->getVertexPositions();
        colors = quads[i]->getVertexColors();
        for (int j = 0; j < verticesPerQuad; j++) {
            vertexList.push_back({ positions[j], colors[j] });
            std::cout << positions[j].x << ", " << positions[j].y << std::endl;
        }
    }

    // Create and load vertex buffer
    m_vb = GraphicsEngine::get()->createVertexBuffer();
    UINT size_list = static_cast<UINT>(vertexList.size());

    void* shader_byte_code = nullptr;
    size_t size_shader = 0;
    GraphicsEngine::get()->compileVertexShader(L"VertexShader.hlsl", "vsmain", &shader_byte_code, &size_shader);

    m_vs = GraphicsEngine::get()->createVertexShader(shader_byte_code, size_shader);
    m_vb->load(
        vertexList.data(),
        sizeof(vertex),
        size_list,
        shader_byte_code,
        size_shader
    );

    GraphicsEngine::get()->releaseCompiledShader();

    GraphicsEngine::get()->compilePixelShader(L"PixelShader.hlsl", "psmain", &shader_byte_code, &size_shader);
    m_ps = GraphicsEngine::get()->createPixelShader(shader_byte_code, size_shader);
    GraphicsEngine::get()->releaseCompiledShader();
}

void AppWindow::onUpdate()
{
	Window::onUpdate();
	GraphicsEngine::get()->getImmediateDeviceContext()->clearRenderTargetColor(this->m_swap_chain,
		0, 0.3f,0.4f, 1);
	RECT rc = this->getClientWindowRect();
	GraphicsEngine::get()->getImmediateDeviceContext()->setViewportSize(rc.right - rc.left, rc.bottom - rc.top);
	GraphicsEngine::get()->getImmediateDeviceContext()->setVertexShader(m_vs);
	GraphicsEngine::get()->getImmediateDeviceContext()->setPixelShader(m_ps);


	GraphicsEngine::get()->getImmediateDeviceContext()->setVertexBuffer(m_vb);

	GraphicsEngine::get()->getImmediateDeviceContext()->drawTriangleStrip(m_vb->getSizeVertexList(), 0);
	m_swap_chain->present(true);
}

void AppWindow::onDestroy()
{
	Window::onDestroy();
	m_vb->release();
	m_swap_chain->release();
	m_vs->release();
	m_ps->release();
	GraphicsEngine::get()->release();
}
