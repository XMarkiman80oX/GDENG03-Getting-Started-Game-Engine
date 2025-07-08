#include "Texture.h"
#include "GraphicsEngine.h"
//#define STB_IMAGE_IMPLEMENTATION
#include "stb-master/stb_image.h"

Texture::Texture(const std::string& filePath)
{
    int channels;
    unsigned char* image_data = stbi_load(filePath.c_str(), &m_width, &m_height, &channels, 4);

    if (image_data)
    {
        D3D11_TEXTURE2D_DESC desc = {};
        desc.Width = m_width;
        desc.Height = m_height;
        desc.MipLevels = 1;
        desc.ArraySize = 1;
        desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
        desc.SampleDesc.Count = 1;
        desc.Usage = D3D11_USAGE_DEFAULT;
        desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
        desc.CPUAccessFlags = 0;

        D3D11_SUBRESOURCE_DATA subresource_data = {};
        subresource_data.pSysMem = image_data;
        subresource_data.SysMemPitch = desc.Width * 4;

        ID3D11Texture2D* texture = nullptr;
        GraphicsEngine::get()->getDevice()->CreateTexture2D(&desc, &subresource_data, &texture);

        if (texture)
        {
            D3D11_SHADER_RESOURCE_VIEW_DESC srv_desc = {};
            srv_desc.Format = desc.Format;
            srv_desc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
            srv_desc.Texture2D.MipLevels = 1;
            GraphicsEngine::get()->getDevice()->CreateShaderResourceView(texture, &srv_desc, &m_srv);
            texture->Release();
        }

        stbi_image_free(image_data);
    }
}

Texture::~Texture()
{
    if (m_srv)
    {
        m_srv->Release();
    }
}

ID3D11ShaderResourceView* Texture::getShaderResourceView()
{
    return m_srv;
}

int Texture::getWidth()
{
    return m_width;
}

int Texture::getHeight()
{
    return m_height;
}