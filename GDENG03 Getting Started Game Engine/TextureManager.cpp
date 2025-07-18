#include "TextureManager.h"
#include "Texture.h"
#include "MeshManager.h"

TextureManager::TextureManager() : ResourceManager()
{
}

TextureManager::~TextureManager()
{
}

TexturePtr TextureManager::createTextureFromFile(const wchar_t* file_path)
{
	return std::static_pointer_cast<Texture>(createResourceFromFile(file_path));
}

TexturePtr TextureManager::createTextureFromPixelData(const void* pixel_data, UINT width, UINT height)
{
	Texture* tex = nullptr;
	try {
		tex = new Texture(pixel_data, width, height);
	}
	catch (...) {
	}

	if (tex)
	{
		return TexturePtr(tex);
	}

	return nullptr;
}

Resource* TextureManager::createResourceFromFileConcrete(const wchar_t* file_path)
{
	Texture* tex = nullptr;
	try {
		tex = new Texture(file_path);
		
	}
	catch(...){
	}
	return tex;
}
