#include "CircleManager.h"

CircleManager::CircleManager()
{
    this->defaultSegmentCount = 144; // Keep default segments
    // this->defaultRadius = 0.75f; // Removed, radius is now a parameter
    this->allCircleVertices = {};
}

CircleManager::~CircleManager()
{
    releaseCircles(); // Ensure circles are deleted
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

std::vector<Circle*> CircleManager::getCircles() // Make sure this method exists and is public
{
    return this->circles; 
}

void CircleManager::rebuildAllCircleVertices()
{
    this->allCircleVertices.clear(); //
    for (Circle* circle : this->circles) //
    {
        // Each circle should have already updated its internal vertices via
        // circle->setScreenAspectRatio() -> randomizeMovementTarget() -> GenerateVertices()
        for (const newVertex& vert : circle->GetCircleVertices()) //
        {
            this->allCircleVertices.push_back(vert); //
        }
    }
    std::cout << "Rebuilt all circle vertices for CircleManager due to aspect ratio change. Total vertices: " << this->allCircleVertices.size() << std::endl;
}

int CircleManager::getDefaultSegmentCount()
{
    return this->defaultSegmentCount;
}

std::vector<newVertex> CircleManager::getAllCircleVertices()
{
    return this->allCircleVertices;
}

// Modified to accept radius
void CircleManager::spawnCircle(vec3 position, vec3 color, float screenAspectRatio, float radius)
{
    // Use the passed radius
    Circle* newCircle = new Circle(this->defaultSegmentCount, radius, position, color, screenAspectRatio);
    this->circles.push_back(newCircle);

    // Add new circle's vertices to the master list
    for (const newVertex& element : newCircle->GetCircleVertices())
    {
        this->allCircleVertices.push_back(element);
    }
    std::cout << "Spawned circle. Total vertices: " << this->allCircleVertices.size() << std::endl;
}

void CircleManager::releaseCircles()
{
    for (Circle* circle : circles) {
        delete circle; // Important: free the memory for each Circle object
    }
    this->circles.clear();
    this->allCircleVertices.clear();
    std::cout << "Released all circles. Total vertices: " << this->allCircleVertices.size() << std::endl;
}

void CircleManager::popCircle()
{
    if (!this->circles.empty()) {
        delete this->circles.back(); // Free memory of the popped circle
        this->circles.pop_back();

        // Rebuild allCircleVertices from remaining circles
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

// This method now tells each circle to update its movement target and regenerate its vertices.
// Then it collects all vertices again.
void CircleManager::randomizeNewPositions()
{
    this->allCircleVertices.clear(); // Clear the old master list

    for (Circle* circle : this->circles)
    {
        circle->randomizeMovementTarget(); // Each circle gets a new target
        circle->GenerateVertices();        // Regenerate its vertices based on the new target

        // Add the updated vertices of this circle to the master list
        for (const newVertex& vert : circle->GetCircleVertices())
        {
            this->allCircleVertices.push_back(vert);
        }
    }
    std::cout << "Randomized new positions for all circles. Total vertices: " << this->allCircleVertices.size() << std::endl;
}