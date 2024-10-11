#include "CircleManager.h"

CircleManager::CircleManager()
{
	this->defaultSegmentCount = 30;
	this->defaultRadius = 0.15f;
	this->allCircleVertices = {};
}

CircleManager::~CircleManager()
{
}

CircleManager* CircleManager::get()
{
	static CircleManager circleManager;
	return &circleManager;
}

int CircleManager::getCircleCount()
{
	return this->circles.size();
}

std::vector<Circle*> CircleManager::getCircles()
{
	return this->circles;
}

int CircleManager::getDefaultSegmentCount()
{
	return this->defaultSegmentCount;
}

std::vector<newVertex> CircleManager::getAllCircleVertices()
{
	return this->allCircleVertices;
}

void CircleManager::spawnCircle(vec3 position, vec3 color, float screenAspectRatio)
{
	Circle* newCircle = new Circle(this->defaultSegmentCount, this->defaultRadius, position, color, screenAspectRatio);
	this->circles.push_back(newCircle);

	for (const newVertex& element : newCircle->GetCircleVertices()) 
	{
		this->allCircleVertices.push_back(element);
	}

	std::cout << this->allCircleVertices.size() << std::endl;
}
