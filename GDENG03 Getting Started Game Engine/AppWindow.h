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
#include "InputListener.h"
#include "Vector3D.h"	

class AppWindow : public Window, public InputListener
{
public:
	AppWindow();
	~AppWindow();

	// Inherited via Window
	virtual void onCreate() override;
	virtual void onUpdate() override;
	virtual void onDestroy() override;
	virtual void onFocus() override;
	virtual void onKillFocus() override;
	// Inherited via InputListener
	void onKeyDown(int key) override;
	void onKeyUp(int key) override;

	// Inherited via InputListener
	void onMouseMove(const Point& deltaMousePosition) override;

	// Inherited via InputListener
	void onLeftMouseDown(const Point& mousePosition) override;
	void onLeftMouseUp(const Point& mousePosition) override;
	void onRightMouseDown(const Point& mousePosition) override;
	void onRightMouseUp(const Point& mousePosition) override;

private:
	void update();

private:
	SwapChain* m_swap_chain;
	VertexBuffer* m_vertex_buffer;
	VertexShader* m_vertex_shader;
	PixelShader* m_pixel_shader;
	ConstantBuffer* m_constant_buffer;
	IndexBuffer* m_index_buffer;
private:
	float m_old_delta;
	float m_new_delta;
	float m_delta_time;

	float m_delta_pos;
	float m_delta_scale;

	float rotationX = 0.0f;
	float rotationY = 0.0f;

	const float rotationSpeedMultiplier = 3.14f;

	Vector3D cubeScale = Vector3D(1.0f);
};