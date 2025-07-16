#include "ConstantBuffer.h"
#include "DeviceContext.h"
#include "RenderSystem.h"
#include <exception>

ConstantBuffer::ConstantBuffer(RenderSystem* system, void* buffer, UINT size_buffer) : m_render_system(system)
{
	D3D11_BUFFER_DESC buff_desc = {};
	buff_desc.Usage = D3D11_USAGE_DEFAULT;
	buff_desc.ByteWidth = size_buffer;
	buff_desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	buff_desc.CPUAccessFlags = 0;
	buff_desc.MiscFlags = 0;

	D3D11_SUBRESOURCE_DATA init_data = {};
	init_data.pSysMem = buffer;

	if (FAILED(this->m_render_system->m_d3d_device->CreateBuffer(&buff_desc, &init_data, &m_buffer)))
	{
		throw::std::exception("Failed to create constant buffer.");
	}
}
void ConstantBuffer::update(DeviceContext* context, void* buffer)
{
	//This will allow us "to upload the new data into our constant buffer in video memory".
	context->m_device_context->UpdateSubresource(this->m_buffer, NULL, NULL, buffer, NULL, NULL);
}

ConstantBuffer::~ConstantBuffer()
{
	if (this->m_buffer) this->m_buffer->Release();
}
