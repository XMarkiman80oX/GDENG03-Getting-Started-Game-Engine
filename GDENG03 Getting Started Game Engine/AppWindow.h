#pragma once
#include "Window.h"
#include "GraphicsEngine.h"
#include "SwapChain.h"
#include "DeviceContext.h"
#include "VertexBuffer.h"
#include "ConstantBuffer.h"
#include "IndexBuffer.h"
#include "VertexShader.h"
#include "PixelShader.h"
#include "Cube.h"

class AppWindow : public Window
{
public:
	AppWindow();
	~AppWindow();

	// Inherited via Window
	virtual void onCreate() override;
	virtual void onUpdate() override;
	virtual void onDestroy() override;
private:
	void update();

private:
	SwapChain* m_swap_chain;
	VertexShader* m_vertex_shader;
	PixelShader* m_pixel_shader;

	// Replaced single cube with an array for 100 cubes
	static const int CUBE_COUNT = 100;
	Cube* m_cubes[CUBE_COUNT];

private:
	float m_old_delta;
	float m_new_delta;
	float m_delta_time;

	float m_delta_pos;
	float m_delta_scale;
};