#pragma once
#include <d3d11.h>

class SwapChain;
class DeviceContext;
class VertexBuffer;
class ConstantBuffer;
class VertexShader;
class PixelShader;

class GraphicsEngine
{
public:
	GraphicsEngine();
	//Initialize the GraphicsEngine and DirectX 11 Device
	bool init();
	//Release all the resources loaded
	bool release();
	~GraphicsEngine();
public:
	SwapChain* createSwapChain();
	DeviceContext* getImmediateDeviceContext();
	VertexBuffer* createVertexBuffer(); 
	ConstantBuffer* createConstantBuffer();
	VertexShader* createVertexShader(const void* shader_byte_code, size_t byte_code_size); 
	PixelShader* createPixelShader(const void* shader_byte_code, size_t byte_code_size); 

	bool isMSAAEnabled() const;
	UINT getMSAASampleCount() const;
	UINT getMSAAQualityLevels() const;
	void toggleMSAA(HWND hwnd, UINT width, UINT height); // Method to toggle MSAA

public:
	bool compileVertexShader(const wchar_t* file_name, const char* entry_point_name, void** shader_byte_code, size_t* byte_code_size);
	bool compilePixelShader(const wchar_t* file_name, const char* entry_point_name, void** shader_byte_code, size_t* byte_code_size);
	
	void releaseCompiledShader();
	// Add a public method to allow AppWindow to reinitialize the SwapChain
    // This might involve more than just the swap chain if other resources depend on it
	bool reinitializeSwapChain(HWND hwnd, UINT width, UINT height, SwapChain*& swapChain);

public:
	static GraphicsEngine* get();

private:
	DeviceContext* m_imm_device_context;

private:
	ID3D11Device* m_d3d_device;
	D3D_FEATURE_LEVEL m_feature_level; 
	bool m_msaaEnabled = false; // Current MSAA state
	UINT m_msaaSampleCount = 1;    // Default to no MSAA
	UINT m_msaaQualityLevels = 0;  // To be determined by hardware support

private:
	IDXGIDevice* m_dxgi_device;
	IDXGIAdapter* m_dxgi_adapter;
	IDXGIFactory* m_dxgi_factory;
	ID3D11DeviceContext* m_imm_context;

private:
	ID3DBlob* m_blob = nullptr;
	ID3DBlob* m_vsblob = nullptr;
	ID3DBlob* m_psblob = nullptr;
	ID3D11VertexShader* m_vertex_shader = nullptr;
	ID3D11PixelShader* m_pixel_shader = nullptr;

private:
	friend class SwapChain;
	friend class VertexBuffer;
	friend class ConstantBuffer;
	friend class VertexShader;
	friend class PixelShader;
};

