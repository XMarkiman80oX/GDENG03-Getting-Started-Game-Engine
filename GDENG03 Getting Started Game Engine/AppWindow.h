#pragma once
#include "Window.h"
#include "GraphicsEngine.h"
#include "SwapChain.h"
#include "DeviceContext.h"
#include "VertexBuffer.h"
#include "DepthBuffer.h"
#include "ConstantBuffer.h"
#include "IndexBuffer.h"
#include "VertexShader.h"
#include "PixelShader.h"
#include "InputListener.h"
#include "Vector3D.h"	
#include "Matrix4x4.h"	
#include "Prerequisites.h"	
#include "GameObjectManager.h"	
#include "WorldCamera.h"	
#include <vector>
#include <memory>
#include "System.h"


class AppWindow : public Window, public InputListener
{
private:
	//float oldDelta;
	float newDelta;
	float deltaTime;

	float deltaPosition;
	float deltaScale;

	bool invertedIsOn = false;
	bool cursorIsVisible = false;

	int objectSelectedIndex = 0;
	std::shared_ptr<System> renderSystemECS;


public:
	static AppWindow* getInstance();

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
	void onMouseMove(const Point& mousePosition) override;

	// Inherited via InputListener
	void onLeftMouseDown(const Point& mousePosition) override;
	void onLeftMouseUp(const Point& mousePosition) override;
	void onRightMouseDown(const Point& mousePosition) override;
	void onRightMouseUp(const Point& mousePosition) override;

private:
	SwapChainPtr m_swap_chain;
	VertexBufferPtr m_vertex_buffer;
	VertexShaderPtr m_vertex_shader;
	PixelShaderPtr m_pixel_shader;
	ConstantBufferPtr m_constant_buffer;
	IndexBufferPtr m_index_buffer;
	DepthBufferPtr m_depth_buffer;

	MeshPtr m_mesh;
	MeshPtr m_mesh2;
	MeshPtr m_mesh3;

	//HELPER FUNCTIONS
private:
	void updateGameObjects(RECT clientWindowRect);
	void destroyGameObjects();
	void selectNextObject();
};