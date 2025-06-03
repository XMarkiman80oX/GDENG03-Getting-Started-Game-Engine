#include "GraphicsEngine.h"
#include "SwapChain.h"
#include "DeviceContext.h"
#include "VertexBuffer.h"
#include "ConstantBuffer.h"
#include "VertexShader.h"
#include "PixelShader.h"

#include <d3dcompiler.h>
#include <iostream>

GraphicsEngine::GraphicsEngine()
{
}

bool GraphicsEngine::init()
{
	D3D_DRIVER_TYPE driver_types[] =
	{
		D3D_DRIVER_TYPE_HARDWARE,
		D3D_DRIVER_TYPE_WARP,
		D3D_DRIVER_TYPE_REFERENCE
	};
	UINT num_driver_types = ARRAYSIZE(driver_types);

	D3D_FEATURE_LEVEL feature_levels[] =
	{
		D3D_FEATURE_LEVEL_11_0
	};
	UINT num_feature_levels = ARRAYSIZE(feature_levels);

	HRESULT res = 0;

	for (UINT driver_type_index = 0; driver_type_index < num_driver_types;)
	{
		res = D3D11CreateDevice(NULL, driver_types[driver_type_index], NULL, D3D11_CREATE_DEVICE_DEBUG /* Enable Debug Layer */, feature_levels,
			num_feature_levels, D3D11_SDK_VERSION, &m_d3d_device, &m_feature_level, &m_imm_context);
		if (SUCCEEDED(res))
		{
			break;
		}
		// It's conventional to increment inside the loop or as part of the for statement's third expression
		++driver_type_index;
	}

	if (FAILED(res))
	{
		std::cout << "D3D11CreateDevice failed!" << std::endl;
		return false;
	}

	m_imm_device_context = new DeviceContext(m_imm_context);

	// Check MSAA support (example: 4x MSAA)
	UINT desiredSampleCount = 8;
	// Ensure m_d3d_device is valid before using it
	if (m_d3d_device) {
		HRESULT hr = m_d3d_device->CheckMultisampleQualityLevels(
			DXGI_FORMAT_R8G8B8A8_UNORM,
			desiredSampleCount,
			&m_msaaQualityLevels
		);

		if (SUCCEEDED(hr) && m_msaaQualityLevels > 0)
		{
			m_msaaSampleCount = desiredSampleCount;
			m_msaaEnabled = false; // Start with MSAA disabled
			std::cout << "MSAA Supported: " << m_msaaSampleCount << "x with " << m_msaaQualityLevels << " quality levels." << std::endl;
		}
		else
		{
			m_msaaSampleCount = 1;
			m_msaaQualityLevels = 0;
			m_msaaEnabled = false;
			std::cout << "MSAA " << desiredSampleCount << "x not supported or no quality levels." << std::endl;
		}
	}
	else {
		// Handle case where m_d3d_device is null, though it shouldn't be if D3D11CreateDevice succeeded
		std::cout << "m_d3d_device is null before MSAA check." << std::endl;
		return false;
	}


	// Get DXGI interfaces
	res = m_d3d_device->QueryInterface(__uuidof(IDXGIDevice), (void**)&m_dxgi_device);
	if (FAILED(res)) {
		std::cout << "Failed to query IDXGIDevice." << std::endl;
		return false;
	}

	res = m_dxgi_device->GetParent(__uuidof(IDXGIAdapter), (void**)&m_dxgi_adapter);
	if (FAILED(res)) {
		std::cout << "Failed to get IDXGIAdapter." << std::endl;
		return false;
	}

	// ***** THIS IS THE CORRECTED LINE *****
	res = m_dxgi_adapter->GetParent(__uuidof(IDXGIFactory), (void**)&m_dxgi_factory);
	if (FAILED(res)) {
		std::cout << "Failed to get IDXGIFactory." << std::endl;
		return false;
	}
	// ***** END OF CORRECTION *****

	return true;
}

bool GraphicsEngine::isMSAAEnabled() const
{
	return m_msaaEnabled;
}

UINT GraphicsEngine::getMSAASampleCount() const
{
	// Return 1 if MSAA is disabled, otherwise the supported sample count
	return m_msaaEnabled ? m_msaaSampleCount : 1;
}

UINT GraphicsEngine::getMSAAQualityLevels() const
{
	// Return 0 if MSAA is disabled or not supported, otherwise quality - 1 (as D3D uses 0 to Quality-1)
	return m_msaaEnabled && m_msaaQualityLevels > 0 ? m_msaaQualityLevels - 1 : 0;
}

void GraphicsEngine::toggleMSAA(HWND hwnd, UINT width, UINT height)
{
	if (m_msaaQualityLevels > 0) // Only allow toggling if MSAA is supported
	{
		m_msaaEnabled = !m_msaaEnabled;
		std::cout << "MSAA Toggled: " << (m_msaaEnabled ? "Enabled" : "Disabled") << std::endl;
		// The actual re-creation of resources will be triggered from AppWindow
	}
	else
	{
		std::cout << "Cannot toggle MSAA: Not supported or no quality levels." << std::endl;
	}
}

// This function will be called by AppWindow to recreate the swap chain
bool GraphicsEngine::reinitializeSwapChain(HWND hwnd, UINT width, UINT height, SwapChain*& swapChain)
{
	if (swapChain)
	{
		swapChain->release(); // Release existing swap chain
		// Important: Ensure m_rtv in SwapChain is also released if it's managed there
	}
	swapChain = createSwapChain();
	// Pass the correct MSAA sample count and quality to the swap chain's init method
	if (!swapChain->init(hwnd, width, height, getMSAASampleCount(), getMSAAQualityLevels()))
	{
		// Handle error
		return false;
	}
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
	if (!SUCCEEDED(D3DCompileFromFile(file_name, nullptr, nullptr, entry_point_name
		, "vs_5_0", 0, 0, &this->m_blob, &error_blob)))
	{
		if(error_blob)error_blob->Release();
		return false;
	}

	*shader_byte_code = m_blob->GetBufferPointer();
	*byte_code_size = m_blob->GetBufferSize();

	return true;
}

bool GraphicsEngine::compilePixelShader(const wchar_t* file_name, const char* entry_point_name, void** shader_byte_code, size_t* byte_code_size)
{
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
