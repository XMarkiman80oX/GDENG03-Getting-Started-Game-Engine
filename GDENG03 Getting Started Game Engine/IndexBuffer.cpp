#include "IndexBuffer.h"
#include "RenderSystem.h"
#include <exception>

IndexBuffer::IndexBuffer(RenderSystem* system, void* list_indices, UINT size_list) : m_render_system (system), m_buffer(0) 
{
	/*
	* This is a descriptor object where we set data relative to our buffer
	*/
	D3D11_BUFFER_DESC buff_desc = {};

	/*
	* Indicates whether our buffer can be accessible by the cpu and gpu
	*/
	buff_desc.Usage = D3D11_USAGE_DEFAULT; // D3D11_USAGE_DEFAULT means it can be read and written in both cpu and gpu

	buff_desc.ByteWidth = 4 * size_list; //Indicates the size in bytes of our buffer
	buff_desc.BindFlags = D3D11_BIND_INDEX_BUFFER; //We tell directx how to bind our buffer to the graphics pipleine
	//Here, we're saying it's a vertex buffer
	buff_desc.CPUAccessFlags = 0; //Not important rn
	buff_desc.MiscFlags = 0;//Not important rn

	D3D11_SUBRESOURCE_DATA init_data = {};
	init_data.pSysMem = list_indices; //passing the pointer in memory in whcih the vertices are located

	m_size_list = size_list;

	//This is where the vertex buffer is created
	if (FAILED(this->m_render_system->m_d3d_device->CreateBuffer(&buff_desc, &init_data, &m_buffer)))
		throw std::exception("Failed to create index buffer.");

}

IndexBuffer::~IndexBuffer() 
{
	this->m_buffer->Release();
}


UINT IndexBuffer::getSizeIndexList()
{
	return this->m_size_list;
}

