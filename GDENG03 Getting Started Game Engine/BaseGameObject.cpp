#include "BaseGameObject.h"

BaseGameObject::BaseGameObject(std::string name) {
	this->name = name;
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