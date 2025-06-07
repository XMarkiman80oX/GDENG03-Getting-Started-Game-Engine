#pragma once
#include "BaseGameObject.h"
#include "ConstantBuffer.h"
#include "DeviceContext.h"
#include "VertexBuffer.h"
#include "IndexBuffer.h"
#include "string"
#include "Vector3D.h"

class Cube : public BaseGameObject
{
private:
	float ticks = 0.0f;
	float deltaPos = 0.0f;
	float deltaTime = 0.0f;
	float speed = 10.0f;
	Vector3D m_rotation_speed;

public:
	Cube(std::string name, void* shaderByteCode, size_t sizeShader);
	~Cube();

public:
	void update(float deltaTime) override;
	void draw(int width, int height, VertexShader* vertexShader, PixelShader* pixelShader) override;
	void setAnimationSpeed(float speed);
	void setRotationSpeed(Vector3D speed);

private:
	VertexBuffer* vertexBuffer;
	IndexBuffer* indexBuffer;
	ConstantBuffer* constantBuffer;

};