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

class Cube : public BaseGameObject
{
protected:
	enum AnimationState
	{
		AS_WARPING,
		AS_TRANSFORMING,
		AS_IDLE
	}; 

	AnimationState animationState = AS_TRANSFORMING;
	float animationTimer = 0.0f;
	Vector3D initialScale = Vector3D(1.0f, 1.0f, 1.0f);
	Vector3D finalScale = Vector3D(5.0f, 0.01f, 5.0f);

	float ticks = 0.0f;
	float deltaPos = 0.0f;
	float speed = 10.0f;

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

protected:
	VertexBuffer* vertexBuffer;
	VertexShader* vertexShader;
	PixelShader* pixelShader;
	IndexBuffer* indexBuffer;
	ConstantBuffer* constantBuffer;

protected: //HELPER FUNCTIONS
	void animate();
	void rotate();

protected:
	void onKeyDown(int key) override;

	void onKeyUp(int key) override;
};