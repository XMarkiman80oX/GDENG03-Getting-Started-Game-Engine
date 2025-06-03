// In Circle.cpp
#include "Circle.h"
#include <vector>
#include <cmath>        // For cos, sin
#include <DirectXMath.h> // For XM_PI, or define M_PI if you prefer cmath's M_PI

#ifndef M_PI // If M_PI is not defined by cmath on your compiler
#define M_PI (3.14159265358979323846)
#endif

// Constructor (should be as you have it)
Circle::Circle(int segmentCount, float radius, vec3 position, vec3 color, float screenAspectRatio)
{
    this->segmentCount = segmentCount;
    this->position = position;
    this->color = color; // This 'color' will be the fill color
    this->radius = radius;
    this->screenAspectRatio = screenAspectRatio;
    this->GenerateVertices();
}

Circle::~Circle()
{
}

void Circle::GenerateVertices()
{
    circleVertices.clear();

    vec3 animationTargetOffsetForAllVertices;
    animationTargetOffsetForAllVertices.randomizeVector(true);

    float effectiveRadiusForClamping = this->radius * 0.8f;
    if (std::abs(animationTargetOffsetForAllVertices.x) == 1.0f) {
        animationTargetOffsetForAllVertices.x -= effectiveRadiusForClamping * animationTargetOffsetForAllVertices.x;
    }
    if (std::abs(animationTargetOffsetForAllVertices.y) == 1.0f) {
        animationTargetOffsetForAllVertices.y -= effectiveRadiusForClamping * animationTargetOffsetForAllVertices.y;
    }

    vec3 centerVertexPos = this->position;
    // 1. Add center vertex
    circleVertices.push_back({ centerVertexPos, centerVertexPos + animationTargetOffsetForAllVertices, this->color });

    float deltaAngle = 2.0f * (float)M_PI / segmentCount;
    vec3 firstCircumferenceVertexPos;

    // 2. Add circumference vertices
    for (int i = 0; i < segmentCount; ++i)
    {
        float angle = (float)i * deltaAngle;
        float x_offset_from_center = this->radius * cos(angle);
        float y_offset_from_center = this->radius * sin(angle);
        float corrected_x_offset = x_offset_from_center / this->screenAspectRatio;

        vec3 currentCircumferencePos = vec3(
            centerVertexPos.x + corrected_x_offset,
            centerVertexPos.y + y_offset_from_center,
            0.0f
        );

        circleVertices.push_back({ currentCircumferencePos, currentCircumferencePos + animationTargetOffsetForAllVertices, this->color });

        if (i == 0)
        {
            firstCircumferenceVertexPos = currentCircumferencePos;
        }
    }

    // 3. Add first circumference vertex again to close the fan
    circleVertices.push_back({ firstCircumferenceVertexPos, firstCircumferenceVertexPos + animationTargetOffsetForAllVertices, this->color });
}

std::vector<newVertex> Circle::GetCircleVertices()
{
    return this->circleVertices;
}