#pragma once
#include <d3d11.h>
#include <string>

class Texture
{
public:
    Texture(const std::string& filePath);
    ~Texture();

    ID3D11ShaderResourceView* getShaderResourceView();
    int getWidth();
    int getHeight();

private:
    ID3D11ShaderResourceView* m_srv = nullptr;
    int m_width = 0;
    int m_height = 0;
};