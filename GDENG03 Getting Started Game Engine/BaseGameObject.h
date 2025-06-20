#pragma once
#include "Matrix4x4.h"
#include "string"
#include <Windows.h>
#include "ConstantBufferData.h"
#include "InputListener.h"
#include "InputSystem.h"

class VertexShader;
class PixelShader;

class BaseGameObject : public InputListener
{
protected:
	struct vertex
	{
		Vector3D position;
		Vector3D color;
		Vector3D color1;
	};

protected: //Inherited to other primitives
	std::string name;
	Vector3D localPosition;
	Vector3D localScale;
	Vector3D localRotation;
	Vector3D localMatrix;

	//If this object is selected, then keybinds will only work on this object
	bool isSelected = false;

public: //Constructors
	BaseGameObject(std::string name);
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


	// Inherited via InputListener
	void onKeyDown(int key) override;

	void onKeyUp(int key) override;

	void onMouseMove(const Point& mousePosition) override;

	void onLeftMouseDown(const Point& mousePosition) override;

	void onLeftMouseUp(const Point& mousePosition) override;

	void onRightMouseDown(const Point& mousePosition) override;

	void onRightMouseUp(const Point& mousePosition) override;

};

