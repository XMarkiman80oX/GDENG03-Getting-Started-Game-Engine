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
#include "EngineTime.h"

class Cube : public BaseGameObject
{
private:
	float ticks = 0.0f;
	float deltaPos = 0.0f;
	float speed = 10.0f;

	Vector3D m_rotation_speed;

public:
	Cube(std::string name, void* shaderByteCode, size_t sizeShader);
	~Cube();

public:
	void update(RECT windowRect) override;
	void draw(int width, int height) override;
	void setAnimationSpeed(float speed);
	void setRotationSpeed(Vector3D speed);
	// Inherited via BaseGameObject
	void initializeObject(void* shaderByteCode, size_t sizeShader) override;

private:
	VertexBuffer* vertexBuffer;
	VertexShader* vertexShader;
	PixelShader* pixelShader;
	IndexBuffer* indexBuffer;
	ConstantBuffer* constantBuffer;

private:
	void onKeyDown(int key) override;

	void onKeyUp(int key) override;
};