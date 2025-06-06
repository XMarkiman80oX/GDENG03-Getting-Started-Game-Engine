#include "DeviceContext.h"
#include "SwapChain.h"
#include "VertexBuffer.h"
#include "ConstantBuffer.h"
#include "VertexShader.h"
#include "PixelShader.h"

DeviceContext::DeviceContext(ID3D11DeviceContext* device_context)
	: m_device_context(device_context)
{
}

void DeviceContext::clearRenderTargetColor(SwapChain* swap_chain, float red, float green, float blue, float alpha)
{
	FLOAT clear_color[] = { red, green, blue, alpha };
	m_device_context->ClearRenderTargetView(swap_chain->m_rtv, clear_color);
	//Will allow us to set which render target we want to draw on, in this case it's the back buffer. 
	m_device_context->OMSetRenderTargets(1, &swap_chain->m_rtv, NULL);
}

void DeviceContext::setVertexBuffer(VertexBuffer* vertex_buffer)
{
	UINT stride = vertex_buffer->m_size_vertex; //Size of our vertex type
	UINT offset = 0;//Size in bytes that allows us to set the beginning of our buffer in another position

	//You're passing the pointer to the buffer and indicating how many buffers will be set.
	//Indicates we only pass one buffer (param 2) and that we have to pass the pointer to the buffer (param 3)
	this->m_device_context->IASetVertexBuffers(0, 1, &vertex_buffer->m_buffer, &stride, &offset);
	
	//We're setting the input layout here.
	this->m_device_context->IASetInputLayout(vertex_buffer->m_layout);
}

/*
  V2-----------V3 V4
  |				/ |
  |	         /    |
  |		  /	      |
  |	   /		  |
  |	/			  |
  V1 V6----------V5
*/
void DeviceContext::drawTriangleList(UINT vertex_count, UINT start_vertex_index)
{
	//We're letting directx know that we're passing a list of triangles
	this->m_device_context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	this->m_device_context->Draw(vertex_count, //how many vertices we want to draw
								start_vertex_index); //then from what vertex we want to start in
}

/*
  V2--------------V4
  |				/ |
  |	         /    |
  |		  /	      |
  |	   /		  |
  |	/			  |
  V1--------------V3
*/
void DeviceContext::drawTriangleStrip(UINT vertex_count, UINT start_vertex_index)
{
	this->m_device_context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);
	this->m_device_context->Draw(vertex_count, start_vertex_index);
}
//We're setting the area or viewportof the already established render target we want to draw on.
void DeviceContext::setViewportSize(UINT width, UINT height)
{
	D3D11_VIEWPORT vp = {};
	vp.Width = (FLOAT)width;
	vp.Height = (FLOAT)height;
	vp.MinDepth = 0.0f;
	vp.MaxDepth = 1.0f;

	m_device_context->RSSetViewports(1, &vp);
}

void DeviceContext::setVertexShader(VertexShader* vertex_shader)
{
	m_device_context->VSSetShader(vertex_shader->m_vertex_shader, nullptr, 0);
}

void DeviceContext::setPixelShader(PixelShader* pixel_shader)
{
	m_device_context->PSSetShader(pixel_shader->m_pixel_shader, nullptr, 0);
}

//Setting constant buffer for the vertex shader
void DeviceContext::setConstantBuffer(VertexShader* vertex_shader, ConstantBuffer* buffer)
{
	this->m_device_context->VSSetConstantBuffers(0, 1, &buffer->m_buffer);
}

//Setting constant buffer for the pixel shader
void DeviceContext::setConstantBuffer(PixelShader* pixel_shader, ConstantBuffer* buffer)
{
	this->m_device_context->PSSetConstantBuffers(0, 1, &buffer->m_buffer);
}

bool DeviceContext::release()
{
	this->m_device_context->Release();
	delete this;
	return true;
}

DeviceContext::~DeviceContext()
{
}
