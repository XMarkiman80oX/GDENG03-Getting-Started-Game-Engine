#include "Circle.h"

Circle::Circle(int segmentCount, float radius, vec3 centerPoint, vec3 color)
{
	this->segmentCount = segmentCount;

	circleVertices[0].position = centerPoint;
	circleVertices[0].color = color;

    this->radius = radius;

	this->GenerateVertices();
}

Circle::~Circle()
{
}

void Circle::GenerateVertices()
{
    // Calculate the vertices on the circumference
    float deltaAngle = 2.0f * (float)DirectX::XM_PI / segmentCount;

    for (int i = 0; i <= segmentCount; ++i)
    {
        float angle = i * deltaAngle;
        float x = radius * cos(angle);
        float y = radius * sin(angle);

        circleVertices.push_back({ vec3(x, y, 0.0f), vec3(0.0f, 0.0f, 1.0f) }); // Blue vertices
    }

}

std::vector<vertex> Circle::GetCircleVertices()
{
    return this->circleVertices;
}
