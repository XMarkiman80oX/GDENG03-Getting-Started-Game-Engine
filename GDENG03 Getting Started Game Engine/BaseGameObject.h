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
#include "GraphicsEngine.h"
#include "IEntity.h"

class BaseGameObject : public IEntity
{
protected: //Inherited to other primitives
	RenderSystem* renderSystem; //Pointer to the render system, so we can access the device and context

	bool hasTexture = true;
	std::string name;
	Vector3D localPosition;
	Vector3D localScale;
	Vector3D localRotation;
	Vector3D localMatrix; 

	const wchar_t* texturePath;
	const wchar_t* meshPath;

	TexturePtr texture;

	//If this object is selected, then keybinds will only work on this object
	bool isSelected = false;

public: //Constructors
	BaseGameObject(
		std::string name,
		RenderSystem* renderSystem,
		const Vector3D& position = Vector3D(),
		const Vector3D& rotation = Vector3D(),
		const Vector3D& scale = Vector3D(1.0f),
		const wchar_t* texturePath = nullptr,
		const wchar_t* meshPath = nullptr
	);
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
	void setTexture(const wchar_t* texturePath);

public: //Getters
	Vector3D getLocalRotation();
	std::string getName();
	const wchar_t* getMeshPath();
	const wchar_t* getTexturePath();
};

