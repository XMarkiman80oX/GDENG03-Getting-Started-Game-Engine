#include "AppWindow.h"
#include <Windows.h>
#include "Vector3D.h"
#include "Matrix4x4.h"
#include "InputSystem.h"
#include "imgui.h"
#include "imgui_impl_dx11.h"
#include "imgui_impl_win32.h"

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
	InputSystem::getInstance()->showCursor(this->cursorIsVisible);

	GraphicsEngine::get()->init();
	this->m_swap_chain = GraphicsEngine::get()->createSwapChain();

	RECT rc = this->getClientWindowRect();
	this->m_swap_chain->init(this->m_hwnd, rc.right - rc.left, rc.bottom - rc.top);

	// Create and initialize the depth buffer
	m_depth_buffer = new DepthBuffer();
	m_depth_buffer->init(rc.right - rc.left, rc.bottom - rc.top);

	this->worldCamera.setTranslation(Vector3D(0, 0, -2));

	//Set the vertices of the object here
	//This is using the triangle strip approach
	vertex vertexList[] = {
		//X - Y - Z
		/***************FRONT FACE****************/
		{Vector3D(-0.5f, -0.75f, -0.5f), //POS1
			Vector3D(1,0,0), Vector3D(0.2f,0,0)},
		{Vector3D(-0.5f, 0.25f, -0.5f),    //POS2
			Vector3D(1,1,0), Vector3D(0.2f,0.2f,0)},
		{Vector3D(0.5f, 0.25f, -0.5f),    //POS3
			Vector3D(1,1,0), Vector3D(0.2f,0.2f,0)},
		{Vector3D(0.5f, -0.75f, -0.5f),     //POS4
			Vector3D(1,0,0), Vector3D(0.2f,0,0)},
			/******************************************/

			/***************BACK FACE****************/
			{Vector3D(0.5f, -0.75f, 0.5f), //POS1
				Vector3D(0,1,0), Vector3D(0,0.2f,0)},
			{Vector3D(0.5f, 0.25f, 0.5f),    //POS2
				Vector3D(0,1,1), Vector3D(0,0.2f,0.2f)},
			{Vector3D(-0.5f, 0.25f, 0.5f),    //POS3
				Vector3D(0,1,1), Vector3D(0,0.2f,0.2f)},
			{Vector3D(-0.5f, -0.75f, 0.5f),     //POS4
				Vector3D(0,1,0), Vector3D(0,0.2f,0)},
				/******************************************/
		/***************PLANE****************/
		//TOP FACE
		{Vector3D(-10.0f, -0.5f, -10.0f), Vector3D(0.8f,0.8f,0.8f), Vector3D(0.8f,0.8f,0.8f)}, // 8
		{Vector3D(-10.0f, -0.5f, 10.0f), Vector3D(0.8f,0.8f,0.8f), Vector3D(0.8f,0.8f,0.8f)}, // 9
		{Vector3D(10.0f, -0.5f, 10.0f), Vector3D(0.8f,0.8f,0.8f), Vector3D(0.8f,0.8f,0.8f)}, // 10
		{Vector3D(10.0f, -0.5f, -10.0f), Vector3D(0.8f,0.8f,0.8f), Vector3D(0.8f,0.8f,0.8f)}, // 11
		//BOTTOM FACE
		{Vector3D(-10.0f, -0.6f, -10.0f), Vector3D(0.8f,0.8f,0.8f), Vector3D(0.8f,0.8f,0.8f)}, // 12
		{Vector3D(-10.0f, -0.6f, 10.0f), Vector3D(0.8f,0.8f,0.8f), Vector3D(0.8f,0.8f,0.8f)}, // 13
		{Vector3D(10.0f, -0.6f, 10.0f), Vector3D(0.8f,0.8f,0.8f), Vector3D(0.8f,0.8f,0.8f)}, // 14
		{Vector3D(10.0f, -0.6f, -10.0f), Vector3D(0.8f,0.8f,0.8f), Vector3D(0.8f,0.8f,0.8f)}, // 15
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
		1,0,7,
		//PLANE
		//Top face
		8,9,10,
		10,11,8,
		//Bottom face
		12,15,14,
		12,14,13,
		//Front face
		13,14,10,
		13,10,9,
		//Back face
		15,12,8,
		15,8,11,
		//Left face
		12,13,9,
		12,9,8,
		//Right face
		14,15,11,
		14,11,10
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


	// Application init: create a dear imgui context, setup some options, load fonts
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO();
	// TODO: Set optional io.ConfigFlags values, e.g. 'io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard' to enable keyboard controls.
	// TODO: Fill optional fields of the io structure later.
	// TODO: Load TTF/OTF fonts if you don't want to use the default font.

	// Initialize helper Platform and Renderer bindings (here we are using imgui_impl_win32.cpp and imgui_impl_dx11.cpp)
	ImGui_ImplWin32_Init(this->m_hwnd);
	ImGui_ImplDX11_Init(GraphicsEngine::get()->getDevice(), GraphicsEngine::get()->getImmediateDeviceContext()->getDeviceContext());

}

void AppWindow::onUpdate()
{
	static bool show_demo_window = true;
	//Window::onUpdate();
	//change color here
	//Inputs get processed here
	InputSystem::getInstance()->update();

	GraphicsEngine::get()->getImmediateDeviceContext()->clearRenderTargetColor(this->m_swap_chain,
		this->m_depth_buffer, 0, 0.3f, 0.4f, 1);
	RECT rc = this->getClientWindowRect();
	GraphicsEngine::get()->getImmediateDeviceContext()->setViewportSize(rc.right - rc.left, rc.bottom - rc.top);

	this->update();

	GraphicsEngine::get()->getImmediateDeviceContext()->setConstantBuffer(this->m_vertex_shader, this->m_constant_buffer);
	GraphicsEngine::get()->getImmediateDeviceContext()->setConstantBuffer(this->m_pixel_shader, this->m_constant_buffer);

	GraphicsEngine::get()->getImmediateDeviceContext()->setVertexShader(this->m_vertex_shader);
	GraphicsEngine::get()->getImmediateDeviceContext()->setPixelShader(this->m_pixel_shader);

	//Here we will pass the vertex buffer from which to get the vertices to render
	GraphicsEngine::get()->getImmediateDeviceContext()->setVertexBuffer(this->m_vertex_buffer);

	GraphicsEngine::get()->getImmediateDeviceContext()->setIndexBuffer(this->m_index_buffer);

	GraphicsEngine::get()->getImmediateDeviceContext()->drawIndexedTriangleList(this->m_index_buffer->getSizeIndexList(), 0, 0);
	m_swap_chain->present(true);

	this->m_old_delta = this->m_new_delta;
	this->m_new_delta = ::GetTickCount();

	//if the old delta has no value, set it to 0 so we dont have a new delta that equals to the new delta one
	this->m_delta_time = (this->m_old_delta) ? ((this->m_new_delta - this->m_old_delta) / 1000.0f) : 0;

	// Feed inputs to dear imgui, start new frame
	ImGui_ImplDX11_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();

	// Any application code here
	ImGui::Text("Hello, world!");

	// Render dear imgui into screen
	ImGui::Render();
	ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
	ImGui::ShowDemoWindow();
	//g_pSwapChain->Present(1, 0);
}

void AppWindow::onDestroy()
{
	Window::onDestroy();
	this->m_vertex_buffer->release();
	this->m_index_buffer->release();
	this->m_constant_buffer->release();
	this->m_depth_buffer->release(); // Release the depth buffer
	this->m_swap_chain->release();

	this->m_vertex_shader->release();
	this->m_pixel_shader->release();
	GraphicsEngine::get()->release();

	// Shutdown
	ImGui_ImplDX11_Shutdown();
	ImGui_ImplWin32_Shutdown();
	ImGui::DestroyContext();
}

void AppWindow::onFocus()
{
	InputSystem::getInstance()->addListener(this);
}

void AppWindow::onKillFocus()
{
	InputSystem::getInstance()->removeListener(this);
}

void AppWindow::update()
{
	constant cc;
	cc.m_time = ::GetTickCount();/*This is a windows function that allows us to get the time elapsed since the
								system started in milliseconds*/

	float movementRate = 1.0f / 0.55f;
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

	//cc.m_world.setScale(this->cubeScale);

	//temp.setIdentity();
	//temp.setRotationZ(0.0f);
	//cc.m_world *= temp;

	//temp.setIdentity();
	//temp.setRotationY(this->rotationY);
	//cc.m_world *= temp;

	//temp.setIdentity();
	//temp.setRotationX(this->rotationX);
	//cc.m_world *= temp;

	cc.m_world.setIdentity();
	float cubeSizeMultiplier = 1 / 300.0f;

	Matrix4x4 worldCam;
	worldCam.setIdentity();

	temp.setIdentity();
	temp.setRotationX(rotationX);
	worldCam *= temp;

	temp.setIdentity();
	temp.setRotationY(rotationY);
	worldCam *= temp;

	//moving through the z axis
	//float value entails how much units is moved
	Vector3D newPos = this->worldCamera.getTranslation() + worldCam.getZDirection() * (this->forward * 0.3f);

	newPos = newPos + worldCam.getXDirection() * (this->rightward * 0.3f);

	//setting our camera backwards two points along the x axis
	worldCam.setTranslation(newPos);
	this->worldCamera = worldCam;
	worldCam.setInverse();

	cc.m_view = worldCam;
	cc.m_proj.setOrthogonalProjectionMatrix(
		(this->getClientWindowRect().right - this->getClientWindowRect().left) * cubeSizeMultiplier,
		(this->getClientWindowRect().bottom - this->getClientWindowRect().top) * cubeSizeMultiplier,
		-4.0f,
		4.0f
	);
	int width = (this->getClientWindowRect().right - this->getClientWindowRect().left);
	int height = (this->getClientWindowRect().bottom - this->getClientWindowRect().top);

	cc.m_proj.setPerspectiveFOVLH(1.57f, (float)width / (float)height, 0.1f, 100.0f);
	this->m_constant_buffer->update(GraphicsEngine::get()->getImmediateDeviceContext(), &cc);
}

void AppWindow::onLeftMouseDown(const Point& mousePosition)
{
	//means xyz are all uniform
	this->cubeScale = Vector3D(0.5f);
}

void AppWindow::onLeftMouseUp(const Point& mousePosition)
{
	this->cubeScale = Vector3D(1.0f);
}

void AppWindow::onRightMouseDown(const Point& mousePosition)
{
	this->cubeScale = Vector3D(2.0f);
}

void AppWindow::onRightMouseUp(const Point& mousePosition)
{
	this->cubeScale = Vector3D(1.0f);
}

void AppWindow::onMouseMove(const Point& mousePosition)
{
	int width = (this->getClientWindowRect().right - this->getClientWindowRect().left);
	int height = (this->getClientWindowRect().bottom - this->getClientWindowRect().top);

	float incrementerX = this->rotationSpeedMultiplier * (mousePosition.x - (width / 2.0f)) * this->m_delta_time;
	float incrementerY = this->rotationSpeedMultiplier * (mousePosition.y - (height / 2.0f)) * this->m_delta_time;

	if (!this->invertedIsOn)
	{
		this->rotationX -= incrementerY;
		this->rotationY -= incrementerX;
	}
	else
	{
		this->rotationX += incrementerY;
		this->rotationY += incrementerX;
	}

	//So it clamps to the mouse's initial position in the window
	InputSystem::getInstance()->setCursorPosition(Point(width / 2.0f, height / 2.0f));
}

void AppWindow::onKeyDown(int key)
{
	switch (key) {
	case 'W':
		//this->rotationX += rotationSpeedMultiplier * this->m_delta_time;
		this->forward = 1.0f;
		break;
	case 'S':
		//this->rotationX -= rotationSpeedMultiplier * this->m_delta_time;
		this->forward = -1.0f;
		break;
	case 'A':
		//this->rotationY += rotationSpeedMultiplier * this->m_delta_time;
		this->rightward = -1.0f;
		break;
	case 'D':
		//this->rotationY -= rotationSpeedMultiplier * this->m_delta_time;
		this->rightward = 1.0f;
		break;
	}
}

void AppWindow::onKeyUp(int key)
{
	//0.0f since we want to stop our camera
	this->forward = 0.0f;
	this->rightward = 0.0f;
}