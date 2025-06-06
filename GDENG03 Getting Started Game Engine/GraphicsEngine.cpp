#include "GraphicsEngine.h"
#include "SwapChain.h"
#include "DeviceContext.h"
#include "VertexBuffer.h"
#include "ConstantBuffer.h"
#include "VertexShader.h"
#include "PixelShader.h"

#include <d3dcompiler.h>

GraphicsEngine::GraphicsEngine()
{
}

bool GraphicsEngine::init()
{
	/* -"allows us to create the device from which we will get access to all the necessary
	* resources necessary to draw on the screen"
	*  -"The driver is what allows directX to exectute the throw in functions"
	*  - We have to loop through some driver types until the creation of the device will be successful
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
	for (UINT driver_type_index = 0; driver_type_index < num_driver_types;)
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
		return false;
	}

	this->m_imm_device_context = new DeviceContext(m_imm_context);

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

	return true;
}

bool GraphicsEngine::release()
{
	if (m_vertex_shader)m_vertex_shader->Release();
	if (m_pixel_shader)m_pixel_shader->Release();

	if (m_vsblob)m_vsblob->Release();
	if (m_psblob)m_psblob->Release();

	this->m_dxgi_device->Release();
	this->m_dxgi_adapter->Release();
	this->m_dxgi_factory->Release();

	this->m_imm_device_context->release();

	this->m_d3d_device->Release();
	return true;
}

GraphicsEngine::~GraphicsEngine()
{
}

SwapChain* GraphicsEngine::createSwapChain()
{
	return new SwapChain();
}

DeviceContext* GraphicsEngine::getImmediateDeviceContext()
{
	return this->m_imm_device_context;
}

VertexBuffer* GraphicsEngine::createVertexBuffer()
{
	return new VertexBuffer();
}

ConstantBuffer* GraphicsEngine::createConstantBuffer()
{
	return new ConstantBuffer();
}

VertexShader* GraphicsEngine::createVertexShader(const void* shader_byte_code, size_t byte_code_size)
{
	VertexShader* vs = new VertexShader();

	if (!vs->init(shader_byte_code, byte_code_size))
	{
		vs->release();
		return nullptr;
	}

	return vs;
}

PixelShader* GraphicsEngine::createPixelShader(const void* shader_byte_code, size_t byte_code_size)
{
	PixelShader* ps = new PixelShader();

	if (!ps->init(shader_byte_code, byte_code_size))
	{
		ps->release();
		return nullptr;
	}

	return ps;
}

bool GraphicsEngine::compileVertexShader(const wchar_t* file_name, const char* entry_point_name, void** shader_byte_code, size_t* byte_code_size)
{
	ID3DBlob* error_blob = nullptr;
	if (!SUCCEEDED(D3DCompileFromFile(file_name, nullptr, nullptr, 
		entry_point_name //the name of the function given by vertex shader hlsl
		, "vs_5_0", //indicates the version of the set of shader features with which we want to 
					//compile our shader code. "vs" for vertex shader.
		0, 0, //useless as of the moment
		&this->m_blob, //output param
						//data structure in which we replaced the buffer with the compiled shader and its size in the memory
		&error_blob))) //output param
						//includes all the warning and error messages in case the compilation fails
	{
		if(error_blob)error_blob->Release();
		return false;
	}

	*shader_byte_code = this->m_blob->GetBufferPointer();
	*byte_code_size = this->m_blob->GetBufferSize();

	return true;
}

bool GraphicsEngine::compilePixelShader(const wchar_t* file_name, const char* entry_point_name, void** shader_byte_code, size_t* byte_code_size)
{
	//Notes are similar in compileVertexShader function
	ID3DBlob* error_blob = nullptr;
	if (!SUCCEEDED(D3DCompileFromFile(file_name, nullptr, nullptr, entry_point_name
		, "ps_5_0", 0, 0, &this->m_blob, &error_blob)))
	{
		if (error_blob)error_blob->Release();
		return false;
	}

	*shader_byte_code = m_blob->GetBufferPointer();
	*byte_code_size = m_blob->GetBufferSize();

	return true;
}

void GraphicsEngine::releaseCompiledShader()
{
	if (this->m_blob)
		m_blob->Release();
}


GraphicsEngine* GraphicsEngine::get()
{
	static GraphicsEngine engine;
	return &engine;
}
