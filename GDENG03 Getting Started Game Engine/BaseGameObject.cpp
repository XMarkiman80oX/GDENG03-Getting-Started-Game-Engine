#include "BaseGameObject.h"
#include "GraphicsEngine.h"

BaseGameObject::BaseGameObject(
	std::string name,
	RenderSystem* renderSystem,
	const Vector3D& position,
	const Vector3D& rotation,
	const Vector3D& scale,
	const wchar_t* texturePath,
	const wchar_t* meshPath)
	:
	name(name),
	renderSystem(renderSystem),
	localPosition(position),
	localScale(rotation),
	localRotation(scale),
	texturePath(texturePath),
	meshPath(meshPath)
{
}
BaseGameObject::~BaseGameObject() {
}

void BaseGameObject::setTexture(const wchar_t* texturePath)
{
	if (this->hasTexture)
	{
		try {
			this->texture = GraphicsEngine::getInstance()->getTextureManager()->createTextureFromFile(texturePath);
			this->texturePath = texturePath;
		}
		catch (const std::exception& e) {
			MessageBox(nullptr, L"Failed to load texture.", L"Error", MB_OK);
		}
	}
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

const wchar_t* BaseGameObject::getMeshPath()
{
	return this->meshPath;
}

const wchar_t* BaseGameObject::getTexturePath()
{
	return this->texturePath;
}
