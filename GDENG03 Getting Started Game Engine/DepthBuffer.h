#pragma once
#include <d3d11.h>

#include "Prerequisites.h"

class DepthBuffer
{
public:
    DepthBuffer(RenderSystem* system);
    ~DepthBuffer();

    bool init(UINT width, UINT height);
    bool release();

private:
    ID3D11Texture2D* m_depth_stencil_buffer;
    ID3D11DepthStencilView* m_depth_stencil_view;
    RenderSystem* m_render_system = nullptr;

private:
    friend class DeviceContext;
    friend class RenderSystem;
};