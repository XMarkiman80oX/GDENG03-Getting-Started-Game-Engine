#include "AppWindow.h"
#include <Windows.h>
#include "Vector3D.h"
#include "Matrix4x4.h"


struct vertex 
{
	Vector3D position;
	Vector3D position1;
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
	this->m_swap_chain->init(this->m_hwnd, rc.right - rc.left /* Width */, rc.bottom - rc.top /* Height */);

	//Set the vertices of the object here
	//This is using the triangle strip approach
	vertex list[] = {
		//X - Y - Z
		{Vector3D( - 0.5f, -0.5f, 0.0f),  Vector3D(-0.32f, -0.11f, 0.0f),      Vector3D(0,1,0),  Vector3D(0,1,0)}, //POS1
		{Vector3D(-0.5f, 0.5f, 0.0f),   Vector3D(-0.11f, 0.78f, 0.0f),       Vector3D(0,1,0), Vector3D(0,1,0)}, //POS2
		{Vector3D(0.5f, -0.5f, 0.0f),    Vector3D(0.75f, -0.73f, 0.0f),      Vector3D(0,1,0), Vector3D(1,0,0)}, //POS3
		{Vector3D(0.5f, 0.5f, 0.0f),     Vector3D(0.88f, 0.77f, 0.0f),       Vector3D(0,1,0), Vector3D(0,0,1)}, //POS4
	};
	//Here we create the vertex buffer, then the established vertex list will be loaded here later on
	this->m_vertex_buffer = GraphicsEngine::get()->createVertexBuffer();
	UINT size_list = ARRAYSIZE(list);

	void* shader_byte_code = nullptr;
	size_t size_shader = 0;
	/*----------------VERTEX SHADER PART----------------*/
	GraphicsEngine::get()->compileVertexShader(L"VertexShader.hlsl", "main", &shader_byte_code, &size_shader);
	
	this->m_vertex_shader = GraphicsEngine::get()->createVertexShader(shader_byte_code, size_shader);
	
	this->m_vertex_buffer->load(list, sizeof(vertex), size_list, shader_byte_code, size_shader);

	GraphicsEngine::get()->releaseCompiledShader();
	/*------------------------------------------------*/

	/*----------------PIXEL SHADER PART----------------*/
	GraphicsEngine::get()->compilePixelShader(L"PixelShader.hlsl", "main", &shader_byte_code, &size_shader);

	this->m_pixel_shader = GraphicsEngine::get()->createPixelShader(shader_byte_code, size_shader);

	GraphicsEngine::get()->releaseCompiledShader();
	/*------------------------------------------------*/

	/*----------------CONSTANT BUFFER PART----------------*/
	constant cc;
	cc.m_time = 0;

	this->m_constant_buffer = GraphicsEngine::get()->createConstantBuffer();
	this->m_constant_buffer->load(&cc, sizeof(constant));
	/*------------------------------------------------*/

}

void AppWindow::onUpdate()
{
	//Window::onUpdate();
	//change color here
	GraphicsEngine::get()->getImmediateDeviceContext()->clearRenderTargetColor(this->m_swap_chain,
		0,0.3f,0.4f,1);
	RECT rc = this->getClientWindowRect();
	GraphicsEngine::get()->getImmediateDeviceContext()->setViewportSize(rc.right - rc.left, rc.bottom - rc.top);
	
	this->updateQuadPosition();

	GraphicsEngine::get()->getImmediateDeviceContext()->setConstantBuffer(m_vertex_shader, m_constant_buffer);
	GraphicsEngine::get()->getImmediateDeviceContext()->setConstantBuffer(m_pixel_shader, m_constant_buffer);

	GraphicsEngine::get()->getImmediateDeviceContext()->setVertexShader(this->m_vertex_shader);
	GraphicsEngine::get()->getImmediateDeviceContext()->setPixelShader(this->m_pixel_shader);

	//Here we will pass the vertex buffer from which to get the vertices to render
	GraphicsEngine::get()->getImmediateDeviceContext()->setVertexBuffer(m_vertex_buffer);

	GraphicsEngine::get()->getImmediateDeviceContext()->drawTriangleStrip(m_vertex_buffer->getSizeVertexList(), 0);
	m_swap_chain->present(true);

	this->m_old_delta = this->m_new_delta;
	this->m_new_delta = ::GetTickCount();

	//if the old delta has no value, set it to 0 so we dont have a new delta that equals to the new delta one
	this->m_delta_time = (this->m_old_delta)?((this->m_new_delta - this->m_old_delta)/1000.0f) :0;
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

void AppWindow::updateQuadPosition()
{
	constant cc;
	cc.m_time = ::GetTickCount();/*This is a windows function that allows us to get the time elapsed since the
								system started in milliseconds*/

	float movementRate = 1.0f/10.0f;
	//This means we reach one unit per 1/movementRate seconds (reciprocal)
	this->m_delta_pos += m_delta_time * movementRate;

	if (this->m_delta_pos > 1.0f) 
		m_delta_pos = 0;
	
	Matrix4x4 temp;
	this->m_delta_scale += this->m_delta_time * movementRate;
	//cc.m_world.setTranslation(Vector3D::lerp(Vector3D(-2.0f, -2.0f, 0.0f), Vector3D(2.0f, 2.0f, 0.0f), this->m_delta_pos));
	cc.m_world.setScale(Vector3D::lerp(Vector3D(0.5f, 0.5f, 0.0f), Vector3D(2.0f, 2.0f, 0.0f), (sin(this->m_delta_scale)+1.0f)/2.0f));
	
	temp.setTranslation(Vector3D::lerp(Vector3D(-1.5f, -1.5f, 0.0f), Vector3D(1.5f, 1.5f, 0.0f), this->m_delta_pos));
	
	cc.m_world *= temp;

	cc.m_view.setIdentity();
	cc.m_proj.setOrthogonalProjectionMatrix(
		(this->getClientWindowRect().right - this->getClientWindowRect().left)/400.0f,
		(this->getClientWindowRect().bottom - this->getClientWindowRect().top)/400.0f,
		-4.0f,
		4.0f
	);

	this->m_constant_buffer->update(GraphicsEngine::get()->getImmediateDeviceContext(), &cc);
}
