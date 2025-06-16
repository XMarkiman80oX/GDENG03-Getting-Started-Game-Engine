#include "VertexShader.h"
#include "GraphicsEngine.h"

VertexShader::VertexShader()
{
}

void VertexShader::release()
{
    this->m_vertex_shader->Release();
    delete this;
}

VertexShader::~VertexShader()
{
}

//The two parameters are not needed at the moment from tutorials 1-9
bool VertexShader::init(const void *shader_byte_code, size_t byte_code_size)
{
    //Remember the last parameter is the attribute of this class, so CreateVertexShader outputs to this attribute to have a reference to the VertexShader
    if(!SUCCEEDED(GraphicsEngine::getInstance()->m_d3d_device->CreateVertexShader(shader_byte_code, byte_code_size, nullptr, &this->m_vertex_shader)))
        return false;

    return true;
}
