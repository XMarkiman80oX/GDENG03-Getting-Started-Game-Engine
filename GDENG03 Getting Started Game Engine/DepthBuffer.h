#pragma once
#include <d3d11.h>

#include "Prerequisites.h"

class DepthBuffer
{
public:
    DepthBuffer(RenderSystem* system, UINT width, UINT height);
    ~DepthBuffer();

private:
    ID3D11Texture2D* m_depth_stencil_buffer = nullptr;
    ID3D11DepthStencilView* m_depth_stencil_view = nullptr;
    RenderSystem* m_render_system = nullptr;

public:
    ID3D11DepthStencilView* getDepthStencilView();

private:
    friend class DeviceContext;
    friend class RenderSystem;
};