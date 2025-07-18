#include "Texture.h"
#include <DirectXTex.h> // For D3DX11CreateShaderResourceViewFromFile
#include "GraphicsEngine.h"

Texture::Texture(const wchar_t* full_path) : Resource(full_path)
{
	DirectX::ScratchImage image_data;
	HRESULT res = DirectX::LoadFromWICFile(full_path, DirectX::WIC_FLAGS_NONE, nullptr, image_data );

	if (SUCCEEDED(res)) {
		res = DirectX::CreateTexture(GraphicsEngine::getInstance()->getRenderSystem()->m_d3d_device, image_data.GetImages(),
			image_data.GetImageCount(), image_data.GetMetadata(), &m_texture);

		D3D11_SHADER_RESOURCE_VIEW_DESC srv_desc = {};
		srv_desc.Format = image_data.GetMetadata().format;
		srv_desc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
		srv_desc.Texture2D.MipLevels = image_data.GetMetadata().mipLevels;
		srv_desc.Texture2D.MostDetailedMip = 0;

		GraphicsEngine::getInstance()->getRenderSystem()->m_d3d_device->CreateShaderResourceView(m_texture,
			&srv_desc, &m_shader_resource_view);
	}
	else {
		throw std::exception("Texture not created successfully");
	}
}

Texture::Texture(const void* pixel_data, UINT width, UINT height) : Resource(L"")
{
	D3D11_TEXTURE2D_DESC tex_desc = {};
	tex_desc.Width = width;
	tex_desc.Height = height;
	tex_desc.MipLevels = 1;
	tex_desc.ArraySize = 1;
	tex_desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	tex_desc.SampleDesc.Count = 1;
	tex_desc.Usage = D3D11_USAGE_DEFAULT;
	tex_desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;

	D3D11_SUBRESOURCE_DATA subresource_data = {};
	subresource_data.pSysMem = pixel_data;
	subresource_data.SysMemPitch = width * 4; // 4 bytes per pixel for R8G8B8A8

	HRESULT res = GraphicsEngine::getInstance()->getRenderSystem()->m_d3d_device->CreateTexture2D(&tex_desc, &subresource_data, (ID3D11Texture2D**)&m_texture);

	if (SUCCEEDED(res))
	{
		D3D11_SHADER_RESOURCE_VIEW_DESC srv_desc = {};
		srv_desc.Format = tex_desc.Format;
		srv_desc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
		srv_desc.Texture2D.MipLevels = 1;
		srv_desc.Texture2D.MostDetailedMip = 0;

		GraphicsEngine::getInstance()->getRenderSystem()->m_d3d_device->CreateShaderResourceView(m_texture, &srv_desc, &m_shader_resource_view);
	}
	else
	{
		throw std::exception("Texture not created successfully from memory");
	}
}

Texture::~Texture()
{
	m_shader_resource_view->Release();
	m_texture->Release();
}
