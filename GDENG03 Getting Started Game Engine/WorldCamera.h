#pragma once
#include "Matrix4x4.h"
class WorldCamera
{
private:
	Matrix4x4 worldCamera;
	Matrix4x4 bufferCamera;

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

	Matrix4x4 getInverseBufferCamera();

public:
	void applyCameraChanges();
	void setRotationX(float rotationValueX);
	void setRotationY(float rotationValueY);

public:
	void moveThroughX(float moveUnits);
	void moveThroughZ(float moveUnits);
	void setInverse();
};

