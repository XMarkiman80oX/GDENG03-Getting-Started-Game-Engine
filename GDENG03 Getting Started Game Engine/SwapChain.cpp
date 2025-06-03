#include "SwapChain.h"
#include "GraphicsEngine.h"

SwapChain::SwapChain()
{
}

bool SwapChain::init(HWND hwnd, UINT width, UINT height, UINT msaaSampleCount, UINT msaaQuality)
{
    ID3D11Device* device = GraphicsEngine::get()->m_d3d_device;

    DXGI_SWAP_CHAIN_DESC desc;
    ZeroMemory(&desc, sizeof(desc));

    desc.BufferCount = 1;
    desc.BufferDesc.Width = width;
    desc.BufferDesc.Height = height;
    desc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    desc.BufferDesc.RefreshRate.Numerator = 60;
    desc.BufferDesc.RefreshRate.Denominator = 1; 
    desc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    desc.OutputWindow = hwnd;

    // Apply MSAA settings
    desc.SampleDesc.Count = msaaSampleCount;
    desc.SampleDesc.Quality = msaaQuality;

    desc.Windowed = TRUE;
    desc.Flags = 0; // No flags like DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH initially

    HRESULT hr = GraphicsEngine::get()->m_dxgi_factory->CreateSwapChain(device, &desc, &m_swap_chain);

    if (FAILED(hr))
        return false;

    ID3D11Texture2D* buffer = NULL;
    hr = m_swap_chain->GetBuffer(0, __uuidof(ID3D11Texture2D), (void**)&buffer);

    if (FAILED(hr))
        return false;

    // Release previous RTV if it exists
    if (m_rtv) {
        m_rtv->Release();
        m_rtv = nullptr;
    }

    // Create Render Target View
    hr = device->CreateRenderTargetView(buffer, NULL, &m_rtv); // m_rtv is a member of SwapChain
    buffer->Release(); // Release the buffer, RTV holds a reference

    if (FAILED(hr))
        return false;

    return true;
}

bool SwapChain::present(bool vsync)
{
    m_swap_chain->Present(vsync, NULL);
    return true;
}

bool SwapChain::release()
{
    if (m_rtv)
    {
        m_rtv->Release();
        m_rtv = nullptr;
    }
    if (m_swap_chain)
    {
        m_swap_chain->Release();
        m_swap_chain = nullptr; // Good practice to nullify after release
    }
    // delete this; // Typically, the owner of SwapChain should delete it.
    // If GraphicsEngine creates it with 'new', then GraphicsEngine or AppWindow should delete it.
    // For now, let's assume the caller of release() will also handle deleting the SwapChain object.
    return true;
}

SwapChain::~SwapChain()
{
}
