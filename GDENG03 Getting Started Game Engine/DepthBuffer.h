#pragma once
#include <d3d11.h>

class GraphicsEngine;
class DeviceContext;

class DepthBuffer
{
public:
    DepthBuffer();
    ~DepthBuffer();

    bool init(UINT width, UINT height);
    bool release();

private:
    ID3D11Texture2D* m_depth_stencil_buffer;
    ID3D11DepthStencilView* m_depth_stencil_view;

private:
    friend class DeviceContext;
    friend class GraphicsEngine;
};