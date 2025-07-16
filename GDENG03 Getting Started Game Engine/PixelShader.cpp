#include "PixelShader.h"
#include "RenderSystem.h"
#include <exception>

PixelShader::PixelShader(RenderSystem* system, const void* shader_byte_code, size_t byte_code_size) : m_render_system(system)
{
    if (!SUCCEEDED(this->m_render_system->m_d3d_device->CreatePixelShader(shader_byte_code, byte_code_size, nullptr, &this->m_pixel_shader)))
        throw std::exception("Failed to render Pixel Shader");

}

PixelShader::~PixelShader()
{
    this->m_pixel_shader->Release();
}

