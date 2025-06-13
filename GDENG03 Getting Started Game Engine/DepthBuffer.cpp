#include "DepthBuffer.h"
#include "GraphicsEngine.h"

DepthBuffer::DepthBuffer()
{
    m_depth_stencil_buffer = nullptr;
    m_depth_stencil_view = nullptr;
}

DepthBuffer::~DepthBuffer()
{
}

bool DepthBuffer::init(UINT width, UINT height)
{
    D3D11_TEXTURE2D_DESC depth_stencil_desc = {};
    depth_stencil_desc.Width = width;
    depth_stencil_desc.Height = height;
    depth_stencil_desc.MipLevels = 1;
    depth_stencil_desc.ArraySize = 1;
    depth_stencil_desc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
    depth_stencil_desc.SampleDesc.Count = 1;
    depth_stencil_desc.SampleDesc.Quality = 0;
    depth_stencil_desc.Usage = D3D11_USAGE_DEFAULT;
    depth_stencil_desc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
    depth_stencil_desc.CPUAccessFlags = 0;
    depth_stencil_desc.MiscFlags = 0;

    HRESULT hr = GraphicsEngine::get()->getDevice()->CreateTexture2D(&depth_stencil_desc, nullptr, &m_depth_stencil_buffer);
    if (FAILED(hr))
    {
        return false;
    }

    hr = GraphicsEngine::get()->getDevice()->CreateDepthStencilView(m_depth_stencil_buffer, nullptr, &m_depth_stencil_view);
    if (FAILED(hr))
    {
        return false;
    }

    return true;
}

bool DepthBuffer::release()
{
    if (m_depth_stencil_view) m_depth_stencil_view->Release();
    if (m_depth_stencil_buffer) m_depth_stencil_buffer->Release();
    delete this;
    return true;
}