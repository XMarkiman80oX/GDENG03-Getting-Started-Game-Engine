#pragma once
#include <d3d11.h>

#include "Prerequisites.h"

class IndexBuffer
{
public:
	IndexBuffer(RenderSystem* system, void* list_indices, UINT size_list);
	UINT getSizeIndexList();
	~IndexBuffer();

private:
	UINT m_size_list;

private:
	ID3D11Buffer* m_buffer;
	RenderSystem* m_render_system = nullptr;

private:
	friend class DeviceContext;
};

