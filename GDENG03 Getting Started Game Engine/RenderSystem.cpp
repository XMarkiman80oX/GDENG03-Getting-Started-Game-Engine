#include "RenderSystem.h"
#include "SwapChain.h"
#include "DeviceContext.h"
#include "VertexBuffer.h"
#include "ConstantBuffer.h"
#include "IndexBuffer.h"
#include "VertexShader.h"
#include "PixelShader.h"
#include "DepthBuffer.h"
#include "TransformComponent.h"
#include "RenderComponent.h"
#include "ConstantBufferData.h"
#include "EngineTime.h"
#include "GraphicsEngine.h"
#include "WorldCamera.h"

#include <d3dcompiler.h>
#include <exception>
#include <iostream>


RenderSystem::RenderSystem()
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

	m_d3d_device->QueryInterface(__uuidof(IDXGIDevice), (void**)&m_dxgi_device);
	m_dxgi_device->GetParent(__uuidof(IDXGIAdapter), (void**)&m_dxgi_adapter);
	m_dxgi_adapter->GetParent(__uuidof(IDXGIFactory), (void**)&m_dxgi_factory);
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

void RenderSystem::draw(int width, int height, System& system)
{
	DeviceContextPtr deviceContext = this->getImmediateDeviceContext();

	for (auto const& entity : system.entities)
	{
		auto& transform = GraphicsEngine::getInstance()->getComponent<TransformComponent>(entity);
		auto& render = GraphicsEngine::getInstance()->getComponent<RenderComponent>(entity);

		deviceContext->setVertexShader(render.vertexShader);
		deviceContext->setPixelShader(render.pixelShader);
		deviceContext->setTexture(render.pixelShader, render.texture);

		constantBufferData cbd = {};
		cbd.m_time = static_cast<unsigned int>(EngineTime::getTotalElapsedTime() * 1000.0);

		Matrix4x4 allMatrix;
		allMatrix.setIdentity();

		Matrix4x4 translationMatrix;
		translationMatrix.setTranslation(transform.position);

		Matrix4x4 scaleMatrix;
		scaleMatrix.setScale(transform.scale);

		Matrix4x4 zMatrix, yMatrix, xMatrix;
		zMatrix.setIdentity();
		yMatrix.setIdentity();
		xMatrix.setIdentity();

		zMatrix.setRotationZ(transform.rotation.z);
		xMatrix.setRotationX(transform.rotation.x);
		yMatrix.setRotationY(transform.rotation.y);

		Matrix4x4 rotationMatrix;
		rotationMatrix = xMatrix * yMatrix * zMatrix;

		allMatrix *= scaleMatrix;
		allMatrix *= rotationMatrix;
		allMatrix *= translationMatrix;

		cbd.m_world = allMatrix;
		cbd.m_view = WorldCamera::getInstance()->getViewMatrix();
		cbd.m_proj = WorldCamera::getInstance()->getProjectionMatrix();

		render.constantBuffer->update(deviceContext, &cbd);
		deviceContext->setConstantBuffer(render.vertexShader, render.constantBuffer);
		deviceContext->setConstantBuffer(render.pixelShader, render.constantBuffer);

		deviceContext->setVertexBuffer(render.vertexBuffer);
		deviceContext->setIndexBuffer(render.indexBuffer);

		deviceContext->drawIndexedTriangleList(render.indexBuffer->getSizeIndexList(), 0, 0);
	}
}
SwapChainPtr RenderSystem::createSwapChain(HWND hwnd, UINT width, UINT height)
{
	SwapChainPtr swap_chain = nullptr;

	try {
		swap_chain = std::make_shared<SwapChain>(this, hwnd, width, height);
	}
	catch (...) {

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
		throw;
	}
	catch (...) {
		std::cerr << "An unknown error occurred while creating the vertex buffer." << std::endl;
		throw;
	}

	return vertex_buffer;
}
ConstantBufferPtr RenderSystem::createConstantBuffer(void* buffer, UINT size_buffer)
{
	ConstantBufferPtr constant_buffer = nullptr;

	try {
		constant_buffer = std::make_shared<ConstantBuffer>(this, buffer, size_buffer);
	}
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