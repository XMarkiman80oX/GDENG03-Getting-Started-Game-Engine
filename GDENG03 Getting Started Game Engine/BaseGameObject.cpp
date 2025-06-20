#include "BaseGameObject.h"

BaseGameObject::BaseGameObject(std::string name) {
	this->name = name;

	//Assumes all game objects have an input component
	//
	InputSystem::getInstance()->addListener(this);
}
BaseGameObject::~BaseGameObject() {
}

void BaseGameObject::setPosition(float x, float y, float z)
{
	this->localPosition = Vector3D(x, y, z);
}

void BaseGameObject::setPosition(Vector3D pos)
{
	this->localPosition = pos;
}

void BaseGameObject::initializeObject(void* shaderByteCode, size_t sizeShader)
{
	InputSystem::getInstance()->addListener(this);
}

Vector3D BaseGameObject::getLocalPosition()
{
	return this->localPosition;
}

void BaseGameObject::setScale(float x, float y, float z)
{
	this->localScale = Vector3D(x, y, z);
}

void BaseGameObject::setScale(Vector3D scale)
{
	this->localScale = scale;
}

Vector3D BaseGameObject::getLocalScale()
{
	return this->localScale;
}

void BaseGameObject::setRotation(float x, float y, float z)
{
	this->localRotation = Vector3D(x, y, z);
}

void BaseGameObject::setRotation(Vector3D rot)
{
	this->localRotation = rot;
}

void BaseGameObject::setSelected(bool isSelected)
{
	this->isSelected = isSelected;

	if(this->isSelected)
		std::cout << this->name << " was Selected" << std::endl;
	/*else 
		std::cout << this->name << " was Deselected" << std::endl;*/
}

Vector3D BaseGameObject::getLocalRotation()
{
	return this->localRotation;
}

std::string BaseGameObject::getName()
{
	return this->name;
}


void BaseGameObject::onKeyDown(int key)
{
}

void BaseGameObject::onKeyUp(int key)
{
}

void BaseGameObject::onMouseMove(const Point& mousePosition)
{
}

void BaseGameObject::onLeftMouseDown(const Point& mousePosition)
{
	if (this->isSelected)
	{
		this->setScale(Vector3D(0.5f));
	}
}

void BaseGameObject::onLeftMouseUp(const Point& mousePosition)
{
	if (this->isSelected) 
	{
		this->setScale(Vector3D(1.0f));
	}
}

void BaseGameObject::onRightMouseDown(const Point& mousePosition)
{
	if (this->isSelected)
	{
		this->setScale(Vector3D(2.0f));
	}
}

void BaseGameObject::onRightMouseUp(const Point& mousePosition)
{
	if (this->isSelected)
	{
		this->setScale(Vector3D(1.0f));
	}
}
