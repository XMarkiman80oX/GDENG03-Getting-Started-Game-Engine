#pragma once
#include <d3d11.h>

#include "Prerequisites.h"

//Immediate Context (not yet deferred)
class DeviceContext
{
public:
	DeviceContext(ID3D11DeviceContext* device_context, RenderSystem* system);
	void clearRenderTargetColor(const SwapChainPtr& swap_chain, float red, float green, float blue, float alpha);
	void setVertexBuffer(const VertexBufferPtr& vertex_buffer);

	void drawIndexedTriangleList(UINT index_count, UINT start_vertex_index, UINT start_index_location);
	void drawTriangleList(UINT vertex_count, UINT start_vertex_index);
	void drawTriangleStrip(UINT vertex_count, UINT start_vertex_index);

	void setViewportSize(UINT width, UINT height);

	void setVertexShader(VertexShaderPtr vertex_shader);
	void setPixelShader(PixelShaderPtr pixel_shader);

	void setTexture(const VertexShaderPtr& vertex_shader, const TexturePtr& texture);
	void setTexture(const PixelShaderPtr& pixel_shader, const TexturePtr& texture);

	void setConstantBuffer(const VertexShaderPtr& vertex_shader, const ConstantBufferPtr& buffer);
	void setConstantBuffer(const PixelShaderPtr& pixel_shader, const ConstantBufferPtr& buffer);

	void setIndexBuffer(const IndexBufferPtr& index_buffer);

	~DeviceContext();

private:
	ID3D11DeviceContext* m_device_context;
	RenderSystem* m_render_system = nullptr;

private:
	friend class ConstantBuffer;
};