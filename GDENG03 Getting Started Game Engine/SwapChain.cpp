#include "SwapChain.h"
#include "GraphicsEngine.h"

SwapChain::SwapChain()
{
}

bool SwapChain::init(HWND hwnd, UINT width, UINT height)
{
    ID3D11Device* device = GraphicsEngine::get()->m_d3d_device;

    /*  DXGI_SWAP_CHAIN_DESC is "A C++ structure in which we have to insert some important values
    * like the handle and size of our window".
    */
    DXGI_SWAP_CHAIN_DESC desc;
    ZeroMemory(&desc, sizeof(desc)); /* -Allows us to fill all the memory occupied by our descriptor with zero
                                        -Basically, it's to be safe to avoid "dirty" initial values*/

    desc.BufferCount = 1; /*In Windowed mode, the swap chain only needs one buffer 
                            since there already is a front buffer (the desktop itself handled by
                            the desktop window manager of the Windows OS)*/

    //Self Explanatory
    desc.BufferDesc.Width = width;
    desc.BufferDesc.Height = height;

    desc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM; /* -This is the pixel format of the frame buffers
                                                            of our swap chain.
                                                            -DXGI_FORMAT_R8G8B8A8_UNORM means 8-bits
                                                            or one byte for each of the 4 color channels
                                                            of our pixels (R, G, B, A).*/
    
    desc.BufferDesc.RefreshRate.Numerator = 60; //Refresh rate
    desc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT; /* Where we decide how to use the buffers of our Swap Chain (to be explained later)
                                                        */
    desc.OutputWindow = hwnd; //We're passing handler of our window

    //The number of multi-samples per pixel (to be talked about later)
    desc.SampleDesc.Count = 1; 
    desc.SampleDesc.Quality = 0; 

    desc.Windowed = TRUE; //Windowed mode or not

    HRESULT hr = GraphicsEngine::get()->m_dxgi_factory->CreateSwapChain(device, &desc, &m_swap_chain);

    if (FAILED(hr))
        return false;

    ID3D11Texture2D* buffer = NULL;
    hr = m_swap_chain->GetBuffer(0, __uuidof(ID3D11Texture2D), (void**)&buffer);

    if (FAILED(hr))
        return false;

    device->CreateRenderTargetView(buffer, NULL, &m_rtv);
    buffer->Release();

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
    this->m_swap_chain->Release();
    delete this;
    return true;
}

SwapChain::~SwapChain()
{
}
