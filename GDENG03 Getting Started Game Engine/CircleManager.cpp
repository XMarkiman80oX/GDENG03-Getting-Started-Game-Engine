#include "CircleManager.h"

CircleManager::CircleManager()
{
    this->defaultSegmentCount = 144; 
    this->allCircleVertices = {};
}

CircleManager::~CircleManager()
{
    releaseCircles(); 
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

void CircleManager::rebuildAllCircleVertices()
{
    this->allCircleVertices.clear(); 
    for (Circle* circle : this->circles) 
    {
        // Each circle should have already updated its internal vertices via
        // circle->setScreenAspectRatio() -> randomizeMovementTarget() -> GenerateVertices()
        for (const newVertex& vert : circle->GetCircleVertices()) //
        {
            this->allCircleVertices.push_back(vert); 
        }
    }
}

int CircleManager::getDefaultSegmentCount()
{
    return this->defaultSegmentCount;
}

std::vector<newVertex> CircleManager::getAllCircleVertices()
{
    return this->allCircleVertices;
}

void CircleManager::spawnCircle(vec3 position, vec3 color, float screenAspectRatio, float radius)
{
    Circle* newCircle = new Circle(this->defaultSegmentCount, radius, position, color, screenAspectRatio);
    this->circles.push_back(newCircle);

    for (const newVertex& element : newCircle->GetCircleVertices())
    {
        this->allCircleVertices.push_back(element);
    }
    std::cout << "Spawned circle. Total vertices: " << this->allCircleVertices.size() << std::endl;
}

void CircleManager::releaseCircles()
{
    for (Circle* circle : circles) 
    {
        delete circle; 
    }
    this->circles.clear();
    this->allCircleVertices.clear();
    std::cout << "Released all circles. Total vertices: " << this->allCircleVertices.size() << std::endl;
}

void CircleManager::popCircle()
{
    if (!this->circles.empty()) {
        delete this->circles.back();
        this->circles.pop_back();

        this->allCircleVertices.clear();
        for (Circle* circle : this->circles)
        {
            for (const newVertex& element : circle->GetCircleVertices())
            {
                this->allCircleVertices.push_back(element);
            }
        }
        std::cout << "Popped circle. Total vertices: " << this->allCircleVertices.size() << std::endl;
    }
}