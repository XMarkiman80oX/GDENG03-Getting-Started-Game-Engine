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
    this->color = color;
    this->radius = radius;
    this->screenAspectRatio = screenAspectRatio;
    this->GenerateVertices(); //
}

Circle::~Circle()
{
}

void Circle::GenerateVertices()
{
    circleVertices.clear();

    vec3 animationTargetOffsetForAllVertices;
    animationTargetOffsetForAllVertices.randomizeVector(true); //

    float effectiveRadiusForClamping = this->radius * 0.8f; //
    if (std::abs(animationTargetOffsetForAllVertices.x) == 1.0f) { //
        animationTargetOffsetForAllVertices.x -= effectiveRadiusForClamping * animationTargetOffsetForAllVertices.x; //
    }
    if (std::abs(animationTargetOffsetForAllVertices.y) == 1.0f) { //
        animationTargetOffsetForAllVertices.y -= effectiveRadiusForClamping * animationTargetOffsetForAllVertices.y; //
    }

    vec3 centerVertexPos = this->position; //
    float deltaAngle = 2.0f * (float)M_PI / segmentCount; //

    for (int i = 0; i < segmentCount; ++i)
    {
        float angle1 = (float)i * deltaAngle;
        float angle2 = (float)(i + 1) * deltaAngle;

        // Current point on circumference (Pi)
        float x_offset1 = this->radius * cos(angle1);
        float y_offset1 = this->radius * sin(angle1);
        float corrected_x_offset1 = x_offset1 / this->screenAspectRatio; //
        vec3 p1 = vec3(
            centerVertexPos.x + corrected_x_offset1,
            centerVertexPos.y + y_offset1,
            0.0f
        );

        // Next point on circumference (P(i+1))
        // For the last segment, P(i+1) will be P0 (cos(2*PI), sin(2*PI))
        float x_offset2 = this->radius * cos(angle2);
        float y_offset2 = this->radius * sin(angle2);
        float corrected_x_offset2 = x_offset2 / this->screenAspectRatio; //
        vec3 p2 = vec3(
            centerVertexPos.x + corrected_x_offset2,
            centerVertexPos.y + y_offset2,
            0.0f
        );

        // Add triangle (Center, Pi, P(i+1))
        // Vertex 1: Center
        circleVertices.push_back({ centerVertexPos, centerVertexPos + animationTargetOffsetForAllVertices, this->color });
        // Vertex 2: Pi
        circleVertices.push_back({ p1, p1 + animationTargetOffsetForAllVertices, this->color });
        // Vertex 3: P(i+1)
        circleVertices.push_back({ p2, p2 + animationTargetOffsetForAllVertices, this->color });
    }
}

std::vector<newVertex> Circle::GetCircleVertices() //
{
    return this->circleVertices; //
}