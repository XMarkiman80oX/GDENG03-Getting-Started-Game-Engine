#include "VertexBuffer.h"
#include "GraphicsEngine.h"

VertexBuffer::VertexBuffer()
	: m_buffer(0), m_layout(0)
{
}

bool VertexBuffer::load(void* list_vertices, UINT size_vertex, UINT size_list, 
	//These are necessary so that the input signature of our shader 
	// can be validated against the array of attributes just created
	//...To be discussed further later on
	void* shader_byte_code, 
	size_t size_byte_shader)
{
	//Since our load method can be used multiple times to load a different list of vertices,
	//we release our resources so that we can create new ones for new lists of vertices.
	if (this->m_buffer)this->m_buffer->Release();
	if (this->m_layout)this->m_layout->Release();

	D3D11_BUFFER_DESC buff_desc = {};
	buff_desc.Usage = D3D11_USAGE_DEFAULT; //Means it can be read and writting in both cpu and gpu
	buff_desc.ByteWidth = size_vertex * size_list; //Size in bytes of our buffer
	buff_desc.BindFlags = D3D11_BIND_VERTEX_BUFFER; //We tell directx how to bind our buffer to the graphics pipleine
													//Here, we're saying it's a vertex buffer
	buff_desc.CPUAccessFlags = 0; //Not important rn
	buff_desc.MiscFlags = 0;//Not important rn

	D3D11_SUBRESOURCE_DATA init_data = {};
	init_data.pSysMem = list_vertices; //passing the pointer in memory in whcih the vertices are located

	this->m_size_vertex = size_vertex;
	this->m_size_list = size_list;

	//This is where the vertex buffer is created
	if(FAILED(GraphicsEngine::get()->m_d3d_device->CreateBuffer(&buff_desc, &init_data, &this->m_buffer)))
		return false;

	//This is a descriptor object, where we add all the info about the attributes composed in our vertex type
	D3D11_INPUT_ELEMENT_DESC layout[] =
	{
		//SEMANTIC NAME - SEMANTIC INDEX - FORMAT - INPUT SLOT - ALIGNED BYTE OFFSET - INPUT SLOT CLASS - INSTANCE DATA STEP RATE
		{"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0},
		{"POSITION", 1, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0},
		{"COLOR", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 24, D3D11_INPUT_PER_VERTEX_DATA, 0},
		{"COLOR", 1, DXGI_FORMAT_R32G32B32_FLOAT, 0, 36, D3D11_INPUT_PER_VERTEX_DATA, 0}
	};
	UINT size_layout = ARRAYSIZE(layout);//returns the number of attributes
	
	//Helps us define the attributes of our vertex type
	if (FAILED(GraphicsEngine::get()->m_d3d_device->CreateInputLayout(layout, size_layout, shader_byte_code, size_byte_shader, &m_layout)))
		return false;
	

	return true;
}

UINT VertexBuffer::getSizeVertexList()
{
	return this->m_size_list;
}

bool VertexBuffer::release()
{
	this->m_layout->Release();
	this->m_buffer->Release();
	delete this;
	return true;
}

VertexBuffer::~VertexBuffer()
{
}
