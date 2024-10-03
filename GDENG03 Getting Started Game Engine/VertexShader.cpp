#include "VertexShader.h"
#include "GraphicsEngine.h"
#include "iostream"

VertexShader::VertexShader()
{
}

void VertexShader::release()
{
    if (this->m_vs) {
        this->m_vs->Release();
        this->m_vs = nullptr; // Set to nullptr after releasing
    }
    delete this; // Be cautious with 'delete this'
}

VertexShader::~VertexShader()
{
}

bool VertexShader::init(const void *shader_byte_code, size_t byte_code_size)
{
    HRESULT hr = GraphicsEngine::get()->m_d3d_device->CreateVertexShader(shader_byte_code, byte_code_size, nullptr, &this->m_vs);
    if (FAILED(hr)) {
        std::cerr << "Failed to create vertex shader. HRESULT: " << hr << std::endl;
        return false;
    }

    return true;
}
