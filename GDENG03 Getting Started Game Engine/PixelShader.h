#pragma once
#include <d3d11.h>

#include "Prerequisites.h"

class PixelShader
{
public:
	PixelShader(RenderSystem* system);
	void release();
	~PixelShader();

private:
	bool init(const void* shader_byte_code, size_t byte_code_size);

private:
	ID3D11PixelShader* m_pixel_shader;
	RenderSystem* m_render_system = nullptr;

private:
	friend class RenderSystem;
	friend class DeviceContext;
};



