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

    circleVertices.push_back({ this->position, vec3(this->position.x + 1.0f, this->position.y, 0.0f), this->color });

    for (int i = 0; i < segmentCount; ++i)
    {
        float angle = i * deltaAngle;
        float x = radius * cos(angle);
        float y = radius * sin(angle);

        x /= this->screenAspectRatio;
        x += position.x;
        y += position.y;

        circleVertices.push_back({ vec3(x, y, 0.0f), vec3(x + 1.0f - this->radius, y, 0.0f), this->color });
    }

    //std::cout << "Inside Circle GenerateVertices(): "<<circleVertices.size() << std::endl;
}

std::vector<newVertex> Circle::GetCircleVertices()
{
    return this->circleVertices;
}
