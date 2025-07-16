#include "VertexBuffer.h"
#include "RenderSystem.h"
#include <exception>

VertexBuffer::VertexBuffer(RenderSystem* system, void* list_vertices, UINT size_vertex, UINT size_list, 
	//These are necessary so that the input signature of our shader 
	// can be validated against the array of attributes just created
	//...To be discussed further later on
	void* shader_byte_code, 
	UINT size_byte_shader) : m_render_system(system)
	,m_buffer(0), m_layout(0)
{
	/*
	* This is a descriptor object where we set data relative to our buffer
	*/
	D3D11_BUFFER_DESC buff_desc = {};

	/*
	* Indicates whether our buffer can be accessible by the cpu and gpu
	*/
	buff_desc.Usage = D3D11_USAGE_DEFAULT; // D3D11_USAGE_DEFAULT means it can be read and written in both cpu and gpu

	buff_desc.ByteWidth = size_vertex * size_list; //Indicates the size in bytes of our buffer
	buff_desc.BindFlags = D3D11_BIND_VERTEX_BUFFER; //We tell directx how to bind our buffer to the graphics pipleine
	//Here, we're saying it's a vertex buffer
	buff_desc.CPUAccessFlags = 0; //Not important rn
	buff_desc.MiscFlags = 0;//Not important rn

	D3D11_SUBRESOURCE_DATA init_data = {};
	init_data.pSysMem = list_vertices; //passing the pointer in memory in which the vertices are located

	m_size_vertex = size_vertex;
	m_size_list = size_list;

	//This is where the vertex buffer is created
	if (FAILED(this->m_render_system->m_d3d_device->CreateBuffer(&buff_desc, &init_data, &m_buffer)))
		throw std::exception("Failed to create vertex buffer.");

	//This is a descriptor object, where we add all the info about the attributes composed in our vertex type
	D3D11_INPUT_ELEMENT_DESC layout[] =
	{
		//SEMANTIC NAME - SEMANTIC INDEX - FORMAT - INPUT SLOT - ALIGNED BYTE OFFSET - INPUT SLOT CLASS - INSTANCE DATA STEP RATE
		{"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0},
		{"TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0}, //only rgb, not rgba
	};
	UINT size_layout = ARRAYSIZE(layout);//returns the number of attributes

	//Helps us define the attributes of our vertex type
	if (FAILED(this->m_render_system->m_d3d_device->CreateInputLayout(layout, size_layout, shader_byte_code, size_byte_shader, &m_layout)))
		throw std::exception("Failed to create input layout.");

}

UINT VertexBuffer::getSizeVertexList()
{
	return this->m_size_list;
}

VertexBuffer::~VertexBuffer()
{
	this->m_layout->Release();
	this->m_buffer->Release();
}
