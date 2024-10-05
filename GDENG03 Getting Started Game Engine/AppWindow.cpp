#include "AppWindow.h"
#include "Quad.h"
#include "iostream"

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

    const int numQuads = 3; // Number of quads to draw
    Quad quads[numQuads];    // Create an array of Quads

    // Adjust positions of each quad (you can set unique colors as well)
    quads[0].setPosition(-1.0f, 0.0f); // Position for first quad
    quads[1].setPosition(0.0f, 0.0f);  // Position for second quad
    quads[2].setPosition(1.0f, 0.0f);  // Position for third quad

    const int verticesPerQuad = 4;
    vertex list[numQuads * verticesPerQuad]; // Array to hold all vertices for all quads

    // Populate the vertex list using Quad's data
    for (int i = 0; i < numQuads; i++)
    {
        Vector3* quadPositions = quads[i].getVertexPositions();
        Vector3* quadColors = quads[i].getVertexColors();

        for (int j = 0; j < verticesPerQuad; j++)
        {
            list[i * verticesPerQuad + j].position = quadPositions[j];
            list[i * verticesPerQuad + j].color = quadColors[j];
        }
    }

    // Create and load the vertex buffer using Quad's data
    m_vb = GraphicsEngine::get()->createVertexBuffer();
    UINT size_list = ARRAYSIZE(list);
    std::cout << "size list: " << size_list << std::endl; //4
    std::cout << "size of vertex: " << sizeof(vertex) << std::endl; //24

    void* shader_byte_code = nullptr;
    size_t size_shader = 0;
    GraphicsEngine::get()->compileVertexShader(L"VertexShader.hlsl", "vsmain", &shader_byte_code, &size_shader);

    m_vs = GraphicsEngine::get()->createVertexShader(shader_byte_code, size_shader);
    m_vb->load(list, sizeof(vertex), size_list, shader_byte_code, size_shader);

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
