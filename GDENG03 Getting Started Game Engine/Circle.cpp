#include "Circle.h"
#include "vector"
#include "cmath"        // For cos, sin, std::abs
#include "algorithm"    // For std::max, std::min
#include "DirectXMath.h"

#ifndef M_PI
#define M_PI (3.14159265358979323846)
#endif

// Constructor remains the same
Circle::Circle(int segmentCount, float radius, vec3 position, vec3 color, float screenAspectRatio)
{
    this->segmentCount = segmentCount;
    this->radius = radius;
    this->position = position;
    this->color = color;
    this->screenAspectRatio = screenAspectRatio; // screenAspectRatio is stored

    this->randomizeMovementTarget();
    this->GenerateVertices();
}

Circle::~Circle()
{
}

void Circle::setScreenAspectRatio(float newAspectRatio)
{
    // Check if the aspect ratio has meaningfully changed
    if (std::abs(this->screenAspectRatio - newAspectRatio) > 0.001f)
    {
        this->screenAspectRatio = newAspectRatio; //

        // The clamping of the movement target depends on the aspect ratio.
        // Re-calculate the movement target with the new aspect ratio.
        this->randomizeMovementTarget(); //

        // The shape of the circle (corrected_x_offset) also depends on the aspect ratio.
        // Regenerate vertices to reflect the new shape and new target.
        this->GenerateVertices(); //
    }
}

void Circle::randomizeMovementTarget()
{
    vec3 random_step_direction;
    // randomizeVector(true) creates an offset where components are typically {-1, 0, 1}
    // or values like (1, 0.5) etc. This determines the direction and magnitude of the raw step.
    random_step_direction.randomizeVector(true);

    // Calculate the potential new center position of the circle
    vec3 potential_target_center = this->position + random_step_direction;

    // Get the effective radius of the circle in X and Y NDC space
    // this->radius is assumed to be in units that match the Y NDC scale (-1 to 1)
    // For the X axis, it needs to be adjusted by the screen aspect ratio
    float effectiveRadiusX = this->radius / this->screenAspectRatio;
    float effectiveRadiusY = this->radius;

    // Clamp the potential_target_center so the circle's bounding box stays within [-1, 1]
    // For X-axis:
    float minX = -1.0f + effectiveRadiusX;
    float maxX = 1.0f - effectiveRadiusX;
    potential_target_center.x = std::max(minX, std::min(potential_target_center.x, maxX));

    // For Y-axis:
    float minY = -1.0f + effectiveRadiusY;
    float maxY = 1.0f - effectiveRadiusY;
    potential_target_center.y = std::max(minY, std::min(potential_target_center.y, maxY));

    potential_target_center.z = 0; // Keep z at 0

    // The actual animation offset is the difference between the clamped target and current position
    this->currentAnimationTargetOffset = potential_target_center - this->position;
}

// GenerateVertices remains the same as in the previous step, using this->currentAnimationTargetOffset
void Circle::GenerateVertices()
{
    circleVertices.clear();

    vec3 centerVertexPos = this->position;
    float deltaAngle = 2.0f * (float)M_PI / segmentCount;

    for (int i = 0; i < segmentCount; i++)
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

        // newPositionDistance is vertex.position (relative to origin) + this->currentAnimationTargetOffset (which is now clamped)
        // More accurately, the shader will use the vertex's original position and this->currentAnimationTargetOffset
        // to determine the target. The 'position' field in newVertex is the original vertex position.
        // The 'newPositionDistance' field in newVertex is 'vertex_original_position + this->currentAnimationTargetOffset'.
        circleVertices.push_back({ centerVertexPos, centerVertexPos + this->currentAnimationTargetOffset, this->color });
        circleVertices.push_back({ p1, p1 + this->currentAnimationTargetOffset, this->color });
        circleVertices.push_back({ p2, p2 + this->currentAnimationTargetOffset, this->color });
    }
}

std::vector<newVertex> Circle::GetCircleVertices()
{
    return this->circleVertices; //
}