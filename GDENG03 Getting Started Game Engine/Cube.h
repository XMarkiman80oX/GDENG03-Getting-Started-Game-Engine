#pragma once
#include "BaseGameObject.h"
#include "ConstantBuffer.h"
#include "DeviceContext.h"
#include "VertexBuffer.h"
#include "IndexBuffer.h"
#include "VertexShader.h"
#include "string"
#include "Vector3D.h"
#include "InputListener.h"
#include "Prerequisites.h"

class Cube : public BaseGameObject, public InputListener
{
private:
	bool hasTexture = true;

	float ticks = 0.0f;
	float deltaPos = 0.0f;
	float deltaTime = 0.0f;
	float speed = 10.0f;

	Vector3D cubeScale = Vector3D(1.0f);
	Vector3D m_rotation_speed;

public:
	Cube(std::string name, void* shaderByteCode, size_t sizeShader, RenderSystem* renderSystem);
	~Cube();

public:
	void update(RECT windowRect) override;
	void draw(int width, int height) override;
	void setAnimationSpeed(float speed);
	void setRotationSpeed(Vector3D speed);
	// Inherited via BaseGameObject
	void initializeObject(void* shaderByteCode, size_t sizeShader) override;

private:
	VertexBufferPtr vertexBuffer;
	VertexShaderPtr vertexShader;
	PixelShaderPtr pixelShader;
	IndexBufferPtr indexBuffer;
	ConstantBufferPtr constantBuffer;

	// Inherited via InputListener
	void onKeyDown(int key) override;
	void onKeyUp(int key) override;
	void onMouseMove(const Point& mousePosition) override;
	void onLeftMouseDown(const Point& mousePosition) override;
	void onLeftMouseUp(const Point& mousePosition) override;
	void onRightMouseDown(const Point& mousePosition) override;
	void onRightMouseUp(const Point& mousePosition) override;

//HELPER FUNCTIONS
private:
	void createTexture(const wchar_t* texturePath);
};