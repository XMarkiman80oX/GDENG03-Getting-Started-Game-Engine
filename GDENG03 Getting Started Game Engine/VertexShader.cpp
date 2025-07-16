#include "VertexShader.h"
#include "RenderSystem.h"
#include <exception>

VertexShader::VertexShader(RenderSystem* system, const void* shader_byte_code, size_t byte_code_size) : m_render_system(system)
{
    if (!SUCCEEDED(this->m_render_system->m_d3d_device->CreateVertexShader(shader_byte_code, byte_code_size, nullptr, &this->m_vertex_shader)))
        throw std::exception("Failed to create vertex shader");
}

VertexShader::~VertexShader()
{
    this->m_vertex_shader->Release();
}
