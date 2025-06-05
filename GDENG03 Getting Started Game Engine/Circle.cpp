#include "Circle.h"
#include <vector>
#include <cmath>
#include <DirectXMath.h>

#ifndef M_PI
#define M_PI (3.14159265358979323846)
#endif

Circle::Circle(int segmentCount, float radius, vec3 position, vec3 color, float screenAspectRatio)
{
    this->segmentCount = segmentCount;
    this->radius = radius;
    this->position = position; // Store the circle's center
    this->color = color;
    this->screenAspectRatio = screenAspectRatio;

    this->randomizeMovementTarget(); // Set an initial random movement target
    this->GenerateVertices();        // Generate vertices based on the initial state
}

Circle::~Circle()
{
}

void Circle::randomizeMovementTarget()
{
    this->currentAnimationTargetOffset.randomizeVector(true); // true for clamping to ends -1 or 1

    // Optional: scale or adjust the offset further if needed
    // For example, to make movements less extreme:
    // this->currentAnimationTargetOffset.x *= 0.5f;
    // this->currentAnimationTargetOffset.y *= 0.5f;


    // Clamping logic as was in the original GenerateVertices, now applied to the member
    // This ensures the target position (position + offset) isn't too far if that's desired.
    // The shader will interpolate towards position + currentAnimationTargetOffset.
    // If randomizeVector(true) clamps to -1/1, and positions are also -1/1, targets could be -2 to 2.
    // The original clamping seemed to be for the offset itself, to make it less than a full screen width/height.
    /*
    float effectiveRadiusForClamping = this->radius * 0.8f; // This logic might need review based on desired effect
    if (std::abs(this->currentAnimationTargetOffset.x) == 1.0f) {
        this->currentAnimationTargetOffset.x -= effectiveRadiusForClamping * this->currentAnimationTargetOffset.x;
    }
    if (std::abs(this->currentAnimationTargetOffset.y) == 1.0f) {
        this->currentAnimationTargetOffset.y -= effectiveRadiusForClamping * this->currentAnimationTargetOffset.y;
    }
    */
}

void Circle::GenerateVertices()
{
    circleVertices.clear();

    // No longer generate animationTargetOffsetForAllVertices here, use this->currentAnimationTargetOffset
    // vec3 animationTargetOffsetForAllVertices;
    // animationTargetOffsetForAllVertices.randomizeVector(true);
    // ... clamping for local animationTargetOffsetForAllVertices removed ...

    vec3 centerVertexPos = this->position; // Use the circle's stored center position
    float deltaAngle = 2.0f * (float)M_PI / segmentCount;

    for (int i = 0; i < segmentCount; ++i)
    {
        float angle1 = (float)i * deltaAngle;
        float angle2 = (float)(i + 1) * deltaAngle;

        float x_offset1 = this->radius * cos(angle1);
        float y_offset1 = this->radius * sin(angle1);
        float corrected_x_offset1 = x_offset1 / this->screenAspectRatio;
        vec3 p1 = vec3(centerVertexPos.x + corrected_x_offset1, centerVertexPos.y + y_offset1, 0.0f);

        float x_offset2 = this->radius * cos(angle2);
        float y_offset2 = this->radius * sin(angle2);
        float corrected_x_offset2 = x_offset2 / this->screenAspectRatio;
        vec3 p2 = vec3(centerVertexPos.x + corrected_x_offset2, centerVertexPos.y + y_offset2, 0.0f);

        // newPositionDistance is vertex.position + this->currentAnimationTargetOffset
        circleVertices.push_back({ centerVertexPos, centerVertexPos + this->currentAnimationTargetOffset, this->color });
        circleVertices.push_back({ p1, p1 + this->currentAnimationTargetOffset, this->color });
        circleVertices.push_back({ p2, p2 + this->currentAnimationTargetOffset, this->color });
    }
}

std::vector<newVertex> Circle::GetCircleVertices()
{
    return this->circleVertices;
}