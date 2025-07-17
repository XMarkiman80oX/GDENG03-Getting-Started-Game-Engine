#include "RenderSystem.h"
#include "SwapChain.h"
#include "DeviceContext.h"
#include "VertexBuffer.h"
#include "ConstantBuffer.h"
#include "IndexBuffer.h"
#include "VertexShader.h"
#include "PixelShader.h"

#include <d3dcompiler.h>
#include <exception>
#include <iostream>

RenderSystem::RenderSystem()
{/* -"allows us to create the device from which we will get access to all the necessary
	* resources necessary to draw on the screen"
	* -"The driver is what allows directX to exectute the throw in functions"
	* - We have to loop through some driver types until the creation of the device will be successful
	* hence why the vector
	*/
	D3D_DRIVER_TYPE driver_types[] =
	{
		//Arranged best to worst
		D3D_DRIVER_TYPE_HARDWARE, /*Where the drawing calls are executed mainly on the gpu
										to guarantee the best performance
									*/
		D3D_DRIVER_TYPE_WARP, /* Where the draw and calls are all executed on the CPU
								*/
		D3D_DRIVER_TYPE_REFERENCE /* Really slow performance */
	};
	UINT num_driver_types = ARRAYSIZE(driver_types);

	D3D_FEATURE_LEVEL feature_levels[] =
	{
		D3D_FEATURE_LEVEL_11_0
	};
	UINT num_feature_levels = ARRAYSIZE(feature_levels);

	HRESULT res = 0;

	//This loop is for scanning our vector of driver types
	for (UINT driver_type_index = 0; driver_type_index < num_driver_types; ++driver_type_index)
	{
		res = D3D11CreateDevice(NULL, driver_types[driver_type_index], NULL, NULL, feature_levels,
			num_feature_levels, D3D11_SDK_VERSION, &m_d3d_device, &m_feature_level, &m_imm_context);
		if (SUCCEEDED(res))
		{
			break;
			++driver_type_index;
		}
	}
	if (FAILED(res))
	{
		throw std::exception("Failed to create render system");
	}

	this->m_imm_device_context = std::make_shared<DeviceContext>(m_imm_context, this);

	/*
	*	If m_d3d_device supports IDXGIDevice, QueryInterface will
	* return a pointer to it and that pointer will be stored in m_dxgi_device
	*/
	m_d3d_device->QueryInterface(__uuidof(IDXGIDevice), (void**)&m_dxgi_device);
	/*
	*	-IDXGIAdapter is basically the GPU or the hardware
	*	-This queries info about the hardware and its capabilities
	*	-The result of this call will give you a pointer to IDXGIAdapter, stored in m_dxgi_adapter.
	*/
	m_dxgi_device->GetParent(__uuidof(IDXGIAdapter), (void**)&m_dxgi_adapter);
	/*
	*	-The IDXGIFactory interface allows you to create and manage resources related to DirectX,
	such as swap chains (which manage buffers for rendering and presenting frames on the screen).
		-The result of this call will give you a pointer to IDXGIFactory, stored in m_dxgi_factory.
	*/
	m_dxgi_adapter->GetParent(__uuidof(IDXGIFactory), (void**)&m_dxgi_factory);
	/*
	* DXGI (DirectX Graphics Infrastructure) provides low-level access to display devices,
	such as video cards and monitors, and is responsible for tasks like
	presenting rendered frames to the screen, managing full-screen transitions, and creating swap chains.

	* -"By accessing the IDXGIDevice, IDXGIAdapter, and IDXGIFactory,
	you can manage these resources and handle things like
	creating swap chains, querying GPU information,
	and handling display modes (windowed vs fullscreen)."
	*/
}

RenderSystem::~RenderSystem()
{
	if (this->m_vertex_shader)this->m_vertex_shader->Release();
	if (this->m_pixel_shader)this->m_pixel_shader->Release();
	if (m_vsblob)m_vsblob->Release();
	if (m_psblob)m_psblob->Release();

	this->m_dxgi_device->Release();
	this->m_dxgi_adapter->Release();
	this->m_dxgi_factory->Release();

	this->m_d3d_device->Release();
}

SwapChainPtr RenderSystem::createSwapChain(HWND hwnd, UINT width, UINT height)
{
	SwapChainPtr swap_chain = nullptr;

	try {
		swap_chain = std::make_shared<SwapChain>(this, hwnd, width, height);
	}
	//if an exception is thrown and caught, the destructor is called automatically and the pointer is not touched
	catch(...){

	}
	return swap_chain;
}

DeviceContextPtr RenderSystem::getImmediateDeviceContext()
{
	return this->m_imm_device_context;
}

VertexBufferPtr RenderSystem::createVertexBuffer(void* list_vertices, UINT size_vertex, UINT size_list, void* shader_byte_code, size_t size_byte_shader)
{
	VertexBufferPtr vertex_buffer = nullptr;

	try {
		vertex_buffer = std::make_shared<VertexBuffer>(this, list_vertices, size_vertex, size_list, shader_byte_code, size_byte_shader);
	}
	catch (const std::runtime_error& e) {
		std::cerr << "ERROR: " << e.what() << std::endl;
		throw; // Re-throw the exception
	}
	catch (...) {
		std::cerr << "An unknown error occurred while creating the vertex buffer." << std::endl;
		throw; // Re-throw the exception
	}

	return vertex_buffer;
}
ConstantBufferPtr RenderSystem::createConstantBuffer(void* buffer, UINT size_buffer)
{
	ConstantBufferPtr constant_buffer = nullptr;

	try {
		constant_buffer = std::make_shared<ConstantBuffer>(this, buffer, size_buffer);
	}
	//if an exception is thrown and caught, the destructor is called automatically and the pointer is not touched
	catch (...) {

	}
	return constant_buffer;
}

DepthBufferPtr RenderSystem::createDepthBuffer(UINT width, UINT height)
{
	DepthBufferPtr depth_buffer = nullptr;

	try {
		depth_buffer = std::make_shared<DepthBuffer>(this, width, height);
	}
	//if an exception is thrown and caught, the destructor is called automatically and the pointer is not touched
	catch (...) {

	}
	return depth_buffer;
}

IndexBufferPtr RenderSystem::createIndexBuffer(void* list_indices, UINT size_list)
{
	IndexBufferPtr index_buffer = nullptr;

	try {
		index_buffer = std::make_shared<IndexBuffer>(this, list_indices, size_list);
	}
	//if an exception is thrown and caught, the destructor is called automatically and the pointer is not touched
	catch (...) {

	}
	return index_buffer;
}

VertexShaderPtr RenderSystem::createVertexShader(const void* shader_byte_code, size_t byte_code_size)
{
	VertexShaderPtr vertex_shader = nullptr;

	try {
		vertex_shader = std::make_shared<VertexShader>(this, shader_byte_code, byte_code_size);
	}
	//if an exception is thrown and caught, the destructor is called automatically and the pointer is not touched
	catch (...) {

	}
	return vertex_shader;
}

PixelShaderPtr RenderSystem::createPixelShader(const void* shader_byte_code, size_t byte_code_size)
{
	PixelShaderPtr pixel_shader = nullptr;

	try {
		pixel_shader = std::make_shared<PixelShader>(this, shader_byte_code, byte_code_size);
	}
	//if an exception is thrown and caught, the destructor is called automatically and the pointer is not touched
	catch (...) {

	}
	return pixel_shader;
}

bool RenderSystem::compileVertexShader(const wchar_t* file_name, const char* entry_point_name, void** shader_byte_code, size_t* byte_code_size)
{
	ID3DBlob* error_blob = nullptr;
	if (!SUCCEEDED(D3DCompileFromFile(file_name, nullptr, nullptr, entry_point_name, "vs_5_0", 0, 0, &m_blob, &error_blob))) {
		if (error_blob) {
			error_blob->Release();
		}
		return false;
	}

	*shader_byte_code = m_blob->GetBufferPointer();
	*byte_code_size = m_blob->GetBufferSize();

	return true;
}

bool RenderSystem::compilePixelShader(const wchar_t* file_name, const char* entry_point_name, void** shader_byte_code, size_t* byte_code_size)
{
	ID3DBlob* error_blob = nullptr;
	if (!SUCCEEDED(D3DCompileFromFile(file_name, nullptr, nullptr, entry_point_name, "ps_5_0", 0, 0, &m_blob, &error_blob))) {
		if (error_blob) {
			error_blob->Release();
		}
		return false;
	}

	*shader_byte_code = m_blob->GetBufferPointer();
	*byte_code_size = m_blob->GetBufferSize();

	return true;
}

void RenderSystem::releaseCompiledShader()
{
	if (this->m_blob)
		m_blob->Release();
}
