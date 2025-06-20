#pragma once
#include <d3d11.h>
#include "Config.h"

class DeviceContext;

//Much like vertex buffer, where it's stored in video memory, however any data can be stored, 
// which will be passed later on to any shader
//in this case, it's the time variable for animation.
class ConstantBuffer
{
public:
	ConstantBuffer();
	bool load(void* buffer, UINT size_buffer);
	void update(DeviceContext* context, void* buffer); //so that we will be able to update our constant buffer
													   //with new content at each render loop.
	bool release();
	~ConstantBuffer();

private:
	ID3D11Buffer* m_buffer;

private:
	friend class DeviceContext;
};

