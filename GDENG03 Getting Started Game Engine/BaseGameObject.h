#pragma once
#include "Matrix4x4.h"
#include "Vector3D.h"
#include "string"
#include <Windows.h>
#include "ConstantBufferData.h"
#include "Prerequisites.h"
#include "RenderSystem.h"
#include <vector>
#include <exception>
#include "EngineTime.h"

class BaseGameObject
{
protected: //Inherited to other primitives
	RenderSystem* renderSystem; //Pointer to the render system, so we can access the device and context

	std::string name;
	Vector3D localPosition;
	Vector3D localScale;
	Vector3D localRotation;
	Vector3D localMatrix;

	TexturePtr texture;

	//If this object is selected, then keybinds will only work on this object
	bool isSelected = false;

public: //Constructors
	BaseGameObject(std::string name, RenderSystem* renderSystem);
	~BaseGameObject();

public: //ABSTRACT CLASSES
	virtual void update(RECT windowRect) = 0;
	virtual void draw(int width, int height) = 0;
	virtual void initializeObject(void* shaderByteCode, size_t sizeShader) = 0;

public: //Setters
	void setPosition(float x, float y, float z);
	void setPosition(Vector3D pos);
	Vector3D getLocalPosition();

	void setScale(float x, float y, float z);
	void setScale(Vector3D scale);
	Vector3D getLocalScale();

	void setRotation(float x, float y, float z);
	void setRotation(Vector3D rot);

	void setSelected(bool isSelected);

public: //Getters
	Vector3D getLocalRotation();
	std::string getName();

};

