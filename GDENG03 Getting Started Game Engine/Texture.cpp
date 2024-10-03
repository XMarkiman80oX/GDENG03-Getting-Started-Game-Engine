#include "Texture.h"
#include <d3dcompiler.h>
#include "GraphicsEngine.h"
#include <WICTextureLoader.h>
#include <iostream>

using namespace DirectX;

Texture::Texture() : m_textureView(nullptr) {}

Texture::~Texture() {
    release();
}

bool Texture::loadFromFile(const std::wstring& filePath) {

    if (!fileExists(filePath)) {  // Ensure this function checks for file existence
        std::wcerr << L"File does not exist: " << filePath << std::endl;
        return false;
    }
    HRESULT hr = CreateWICTextureFromFile(
        GraphicsEngine::get()->m_d3d_device,
        filePath.c_str(),
        nullptr,
        &m_textureView
    );

    if (FAILED(hr)) {
        std::wcerr << L"Failed to load texture file: " << filePath << L" HRESULT: " << hr << std::endl; // Debug output
        return false;
    }
    else std::wcerr << L"File Exists " << std::endl;
    return true;
}

bool Texture::fileExists(const std::wstring& filePath) {
    DWORD fileAttr = GetFileAttributesW(filePath.c_str());
    if (fileAttr == INVALID_FILE_ATTRIBUTES) {
        std::wcerr << L"File not found: " << filePath << std::endl; // Debug output
        return false;
    }
    return !(fileAttr & FILE_ATTRIBUTE_DIRECTORY);
}
void Texture::bind(ID3D11DeviceContext* context) {
    context->PSSetShaderResources(0, 1, &m_textureView);
}

void Texture::release() {
    if (m_textureView) {
        m_textureView->Release();
        m_textureView = nullptr;
    }
}