#include "Circle.h"

Circle::Circle(int segmentCount, float radius, vec3 position, vec3 color, float screenAspectRatio)
{
	this->segmentCount = segmentCount;
    this->position = position;

    this->color = color;
    this->radius = radius;
    this->screenAspectRatio = screenAspectRatio;

	this->GenerateVertices();
}

Circle::~Circle()
{
}

void Circle::GenerateVertices()
{
    float deltaAngle = 2.0f * (float)DirectX::XM_PI / segmentCount;

    vec3 randomizedVector;
    randomizedVector.randomizeVector(true);

    //clamp with radius, else, center would go to the end of screen instead of the edge
    if (randomizedVector.x == 1.0f || randomizedVector.x == -1.0f)
        randomizedVector.x -= this->radius * randomizedVector.x;
    if (randomizedVector.y == 1.0f || randomizedVector.y == -1.0f)
        randomizedVector.y -= this->radius * randomizedVector.y;

    randomizedVector.printVector();


    for (int i = 0; i < segmentCount; ++i)
    {
        float angle = i * deltaAngle;
        float x = radius * cos(angle);
        float y = radius * sin(angle);

        x /= this->screenAspectRatio;
        x += position.x;
        y += position.y;


        circleVertices.push_back({ vec3(x, y, 0.0f), vec3(x, y, 0.0f) + randomizedVector, this->color});
    }

    //std::cout << "Inside Circle GenerateVertices(): "<<circleVertices.size() << std::endl;
}

std::vector<newVertex> Circle::GetCircleVertices()
{
    return this->circleVertices;
}
