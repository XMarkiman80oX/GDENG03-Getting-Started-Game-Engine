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
#include "Matrix4x4.h"	
#include "DepthBuffer.h"	
#include "WorldCamera.h"	
#include "Cube.h"
#include "Plane.h"
#include "Sphere.h"
#include "Config.h"
#include <vector>

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

	std::vector<BaseGameObject*> objectsInWorld = {};

	int objectSelectedIndex = 0;

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
	SwapChain* m_swap_chain;
	VertexBuffer* m_vertex_buffer;
	VertexShader* m_vertex_shader;
	PixelShader* m_pixel_shader;
	ConstantBuffer* m_constant_buffer;
	IndexBuffer* m_index_buffer;
	DepthBuffer* m_depth_buffer;

//HELPER FUNCTIONS
private:
	void updateGameObjects(RECT clientWindowRect);
	void destroyGameObjects();
	void initializeFirstAsSelected();
	void selectNextObject();
	float randomizeFromRange(float lowerBound, float upperBound);
	void spawnHouseOfCards(void* shader_byte_code, size_t size_shader);
};