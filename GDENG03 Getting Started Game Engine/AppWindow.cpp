#include "AppWindow.h"
#include <Windows.h>
#include "Vector3D.h"
#include "Matrix4x4.h"
#include "InputSystem.h"


struct vertex 
{
	Vector3D position;
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
	//We need to add AppWindow as a listener to the Input System
	InputSystem::getInstance()->addListener(this);

	GraphicsEngine::get()->init();
	this->m_swap_chain = GraphicsEngine::get()->createSwapChain();

	RECT rc = this->getClientWindowRect();
	this->m_swap_chain->init(this->m_hwnd, rc.right - rc.left /* Width */, rc.bottom - rc.top /* Height */);

	//Set the vertices of the object here
	//This is using the triangle strip approach
	vertex vertexList[] = {
		//X - Y - Z
		/***************FRONT FACE****************/
		{Vector3D( -0.5f, -0.5f, -0.5f), //POS1
			Vector3D(1,0,0), Vector3D(0.2f,0,0)}, 
		{Vector3D(-0.5f, 0.5f, -0.5f),    //POS2
			Vector3D(1,1,0), Vector3D(0.2f,0.2f,0)},
		{Vector3D(0.5f, 0.5f, -0.5f),    //POS3
			Vector3D(1,1,0), Vector3D(0.2f,0.2f,0)},
		{Vector3D(0.5f, -0.5f, -0.5f),     //POS4
			Vector3D(1,0,0), Vector3D(0.2f,0,0)},
		/******************************************/

		/***************BACK FACE****************/
		{Vector3D(0.5f, -0.5f, 0.5f), //POS1
			Vector3D(0,1,0), Vector3D(0,0.2f,0)},
		{Vector3D(0.5f, 0.5f, 0.5f),    //POS2
			Vector3D(0,1,1), Vector3D(0,0.2f,0.2f)},
		{Vector3D(-0.5f, 0.5f, 0.5f),    //POS3
			Vector3D(0,1,1), Vector3D(0,0.2f,0.2f)},
		{Vector3D(-0.5f, -0.5f, 0.5f),     //POS4
			Vector3D(0,1,0), Vector3D(0,0.2f,0)},
		/******************************************/
	};
	//Here we create the vertex buffer, then the established vertex list will be loaded here later on
	this->m_vertex_buffer = GraphicsEngine::get()->createVertexBuffer();
	UINT size_list = ARRAYSIZE(vertexList);

	/*----------------INDEX BUFFER PART----------------*/
	unsigned int index_list[] = {
		//FRONT SIDE
		0,1,2, // 1st Triangle
		2,3,0, //2nd Triangle
		//BACK SIDE
		4,5,6,
		6,7,4,
		//TOP SIDE
		1,6,5,
		5,2,1,
		//BOTTOM SIDE
		7,0,3,
		3,4,7,
		//RIGHT SIDE
		3,2,5,
		5,4,3,
		//LEFT SIDE
		7,6,1,
		1,0,7
	};
	this->m_index_buffer = GraphicsEngine::get()->createIndexBuffer();
	UINT size_index_list = ARRAYSIZE(index_list);
	this->m_index_buffer->load(index_list, size_index_list);
	/*------------------------------------------------*/

	void* shader_byte_code = nullptr;
	size_t size_shader = 0;
	/*----------------VERTEX SHADER PART----------------*/
	GraphicsEngine::get()->compileVertexShader(L"VertexShader.hlsl", "main", &shader_byte_code, &size_shader);
	
	this->m_vertex_shader = GraphicsEngine::get()->createVertexShader(shader_byte_code, size_shader);
	
	this->m_vertex_buffer->load(vertexList, sizeof(vertex), size_list, shader_byte_code, size_shader);

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
	//Inputs get processed here
	InputSystem::getInstance()->update();

	GraphicsEngine::get()->getImmediateDeviceContext()->clearRenderTargetColor(this->m_swap_chain,
		0,0.3f,0.4f,1);
	RECT rc = this->getClientWindowRect();
	GraphicsEngine::get()->getImmediateDeviceContext()->setViewportSize(rc.right - rc.left, rc.bottom - rc.top);
	
	this->updateQuadPosition();

	GraphicsEngine::get()->getImmediateDeviceContext()->setConstantBuffer(this->m_vertex_shader, this->m_constant_buffer);
	GraphicsEngine::get()->getImmediateDeviceContext()->setConstantBuffer(this->m_pixel_shader, this->m_constant_buffer);

	GraphicsEngine::get()->getImmediateDeviceContext()->setVertexShader(this->m_vertex_shader);
	GraphicsEngine::get()->getImmediateDeviceContext()->setPixelShader(this->m_pixel_shader);

	//Here we will pass the vertex buffer from which to get the vertices to render
	GraphicsEngine::get()->getImmediateDeviceContext()->setVertexBuffer(this->m_vertex_buffer);

	GraphicsEngine::get()->getImmediateDeviceContext()->setIndexBuffer(this->m_index_buffer);

	GraphicsEngine::get()->getImmediateDeviceContext()->drawIndexedTriangleList(this->m_index_buffer->getSizeIndexList(), 0,0);
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
	this->m_index_buffer->release();
	this->m_constant_buffer->release();
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

	float movementRate = 1.0f/0.55f;
	//This means we reach one unit per 1/movementRate seconds (reciprocal)
	this->m_delta_pos += m_delta_time * movementRate;

	if (this->m_delta_pos > 1.0f) 
		m_delta_pos = 0;
	
	Matrix4x4 temp;
	this->m_delta_scale += this->m_delta_time * movementRate;
	//cc.m_world.setTranslation(Vector3D::lerp(Vector3D(-2.0f, -2.0f, 0.0f), Vector3D(2.0f, 2.0f, 0.0f), this->m_delta_pos));
	/*cc.m_world.setScale(Vector3D::lerp(Vector3D(0.5f, 0.5f, 0.0f), Vector3D(2.0f, 2.0f, 0.0f), (sin(this->m_delta_scale)+1.0f)/2.0f));
	
	temp.setTranslation(Vector3D::lerp(Vector3D(-1.5f, -1.5f, 0.0f), Vector3D(1.5f, 1.5f, 0.0f), this->m_delta_pos));
	
	cc.m_world *= temp;*/

	cc.m_world.setScale(Vector3D(1, 1, 1));

	temp.setIdentity();
	temp.setRotationZ(0.0f);
	cc.m_world *= temp;

	temp.setIdentity();
	temp.setRotationY(this->m_rotation_y);
	cc.m_world *= temp;

	temp.setIdentity();
	temp.setRotationX(this->m_rotation_x);
	cc.m_world *= temp;

	float cubeSizeMultiplier = 1 / 300.0f;
	cc.m_view.setIdentity();
	cc.m_proj.setOrthogonalProjectionMatrix(
		(this->getClientWindowRect().right - this->getClientWindowRect().left)*cubeSizeMultiplier,
		(this->getClientWindowRect().bottom - this->getClientWindowRect().top) * cubeSizeMultiplier,
		-4.0f,
		4.0f
	);

	this->m_constant_buffer->update(GraphicsEngine::get()->getImmediateDeviceContext(), &cc);
}

void AppWindow::onKeyDown(int key)
{
	if (key == 'W') {
		this->m_rotation_x += 0.707 * this->m_delta_time;
	}
	else if (key == 'S') {
		this->m_rotation_x -= 0.707 * this->m_delta_time;
	}
	else if (key == 'A') {
		this->m_rotation_y += 0.707 * this->m_delta_time;
	}
	else if (key == 'D') {
		this->m_rotation_y -= 0.707 * this->m_delta_time;
	}
}

void AppWindow::onKeyUp(int key)
{
}
