#include "CircleManager.h"

CircleManager::CircleManager()
{
	this->defaultSegmentCount = 144;
	this->defaultRadius = 0.75f;
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
	//std::cout << "Circle Vertices Size: " << this->allCircleVertices.size() << std::endl;
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

void CircleManager::releaseCircles()
{
	this->circles.clear();
	this->allCircleVertices.clear();
	std::cout << this->allCircleVertices.size() << std::endl;
}

void CircleManager::popCircle()
{
	this->allCircleVertices.clear();

	this->circles.pop_back();

	//Update list
	for (Circle* circle : this->circles) 
	{
		for (const newVertex& element : circle->GetCircleVertices())
		{
			this->allCircleVertices.push_back(element);
		}
	}

	std::cout << this->allCircleVertices.size() << std::endl;
}

void CircleManager::randomizeNewPositions()
{
	for (Circle* circle : this->circles)
	{
		for (int i = 0; i < circle->GetCircleVertices().size(); i++)
		{
			circle->GetCircleVertices()[i].newPositionDistance.randomizeVector(true);
		}
	}
}

