#pragma once
#include "Matrix4x4.h"
#include "InputListener.h"
#include <Windows.h>

class WorldCamera : public InputListener
{
private:
	RECT windowRect;
	Matrix4x4 worldCamera;
	Matrix4x4 m_proj; // Added to store the projection matrix

	bool invertedIsOn = false;

	float rotationX = 0.0f;
	float rotationY = 0.0f;

	//Represents the sign of direction
	float forward = 0.0f;
	float rightward = 0.0f;

	const float sensitivity = 0.14f;

public:
	WorldCamera();
	~WorldCamera();

public:
	static WorldCamera* getInstance();

public:
	Matrix4x4 getWorldCamera();
	Vector3D getTranslation();
	Vector3D getZDirection();
	Vector3D getXDirection();
	Matrix4x4 getViewMatrix();
	Matrix4x4 getProjectionMatrix(); // Added getter for the projection matrix

public:
	void initialize(RECT windowRect);
	void updateCamera();

public:
	void setInverse();

	// Inherited via InputListener
	void onKeyDown(int key) override;
	void onKeyUp(int key) override;
	void onMouseMove(const Point& mousePosition) override;
	void onLeftMouseDown(const Point& mousePosition) override;
	void onLeftMouseUp(const Point& mousePosition) override;
	void onRightMouseDown(const Point& mousePosition) override;
	void onRightMouseUp(const Point& mousePosition) override;
};