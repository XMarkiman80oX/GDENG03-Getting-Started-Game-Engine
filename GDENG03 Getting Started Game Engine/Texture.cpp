#include "Texture.h"
#include <DirectXTex.h> // For D3DX11CreateShaderResourceViewFromFile
#include "GraphicsEngine.h"

Texture::Texture(const wchar_t* full_path) : Resource(full_path)
{
	DirectX::ScratchImage image_data;
	HRESULT res = DirectX::LoadFromWICFile(full_path, DirectX::WIC_FLAGS_NONE, nullptr, image_data );

	if (SUCCEEDED(res)) {
		res = DirectX::CreateTexture(GraphicsEngine::get()->getRenderSystem()->m_d3d_device, image_data.GetImages(),
			image_data.GetImageCount(), image_data.GetMetadata(), &m_texture);

		D3D11_SHADER_RESOURCE_VIEW_DESC srv_desc = {};
		srv_desc.Format = image_data.GetMetadata().format;
		srv_desc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
		srv_desc.Texture2D.MipLevels = image_data.GetMetadata().mipLevels;
		srv_desc.Texture2D.MostDetailedMip = 0;

		GraphicsEngine::get()->getRenderSystem()->m_d3d_device->CreateShaderResourceView(m_texture,
			&srv_desc, &m_shader_resource_view);
	}
	else {
		throw std::exception("Texture not created successfully");
	}
}

Texture::~Texture()
{
	m_shader_resource_view->Release();
	m_texture->Release();
}
