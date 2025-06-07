#pragma once
#include "Window.h"
#include "GraphicsEngine.h"
#include "SwapChain.h"
#include "DeviceContext.h"
#include "VertexBuffer.h"
#include "ConstantBuffer.h"
#include "VertexShader.h"
#include "PixelShader.h"

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
	SwapChain* m_swap_chain;
	VertexBuffer* m_vertex_buffer;
	VertexShader* m_vertex_shader;
	PixelShader* m_pixel_shader;
	ConstantBuffer* m_constant_buffer;

	// Add these for animation control
	const float baseSpeed = 1000.0f;
	float m_time_accumulator = 0.0f;
	float m_current_speed; // Start at base speed
	bool m_is_accelerating = true;

	//Disable for slide 14
	//Enables acceleration and decelaration for slide 13
	bool oscillate_enabled = true;
};