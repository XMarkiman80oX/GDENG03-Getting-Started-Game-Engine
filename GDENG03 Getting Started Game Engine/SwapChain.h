#pragma once
#include <d3d11.h>
#include "Prerequisites.h"

class SwapChain
{
public:
	SwapChain(RenderSystem* system, HWND hwnd, UINT width, UINT height);
	~SwapChain();
	bool present(bool vsync);

private:
	IDXGISwapChain* m_swap_chain;
	ID3D11RenderTargetView* m_render_target_view;
	RenderSystem* m_render_system = nullptr;

private:
	friend class DeviceContext;
};

