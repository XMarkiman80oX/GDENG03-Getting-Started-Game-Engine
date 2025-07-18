#pragma once
#include "ResourceManager.h"
#include <Windows.h> 

class TextureManager : public ResourceManager
{
public:
	TextureManager();
	~TextureManager();
	TexturePtr createTextureFromFile(const wchar_t* file_path);
	//allows for basic fill colors, not just images
	TexturePtr createTextureFromPixelData(const void* pixel_data, UINT width, UINT height);

protected:
	// Inherited via ResourceManager
	virtual Resource* createResourceFromFileConcrete(const wchar_t* file_path);
};

