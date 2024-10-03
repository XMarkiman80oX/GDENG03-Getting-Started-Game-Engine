#pragma once
#include <d3d11.h>
#include <string>

class Texture {
public:
    Texture();
    ~Texture();

    bool loadFromFile(const std::wstring& filePath);
    void bind(ID3D11DeviceContext* context);
    void release();

private:
    ID3D11ShaderResourceView* m_textureView;

public:
    bool fileExists(const std::wstring& filePath);
};