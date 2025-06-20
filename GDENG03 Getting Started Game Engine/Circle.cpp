#include "Circle.h"
#include "vector"
#include "cmath"
#include "algorithm"
#include "DirectXMath.h"

#ifndef M_PI
#define M_PI (3.1415)
#endif

// Constructor updated to call initializeVelocity
Circle::Circle(int segmentCount, float radius, vec3 position, vec3 color, float screenAspectRatio)
{
    this->segmentCount = segmentCount;
    this->radius = radius;
    this->position = position;
    this->color = color;
    this->screenAspectRatio = screenAspectRatio;

    this->initializeVelocity(); // Set initial random velocity
    this->GenerateVertices();
}

Circle::~Circle()
{
}

void Circle::setScreenAspectRatio(float newAspectRatio)
{
    if (std::abs(this->screenAspectRatio - newAspectRatio) > 0.001f)
    {
        this->screenAspectRatio = newAspectRatio;
        this->GenerateVertices();
    }
}

//Randomizes velocity
void Circle::initializeVelocity()
{
    vec3 random_direction;
    // Get a random direction with components between -0.5 and 0.5
    random_direction.randomizeVector(false, -1.5f, 5.5f);
    this->velocity = random_direction;

    this->velocity.printVector();
}

//For handling movement and bouncing
void Circle::update(float deltaTime)
{
    float effectiveRadiusX = this->radius / this->screenAspectRatio;
    float effectiveRadiusY = this->radius;

    if ((this->position.x + effectiveRadiusX > 1.0f && this->velocity.x > 0) ||
        (this->position.x - effectiveRadiusX < -1.0f && this->velocity.x < 0))
    {
        this->velocity.x *= -1.0f;
    }

    if ((this->position.y + effectiveRadiusY > 1.0f && this->velocity.y > 0) ||
        (this->position.y - effectiveRadiusY < -1.0f && this->velocity.y < 0))
    {
        this->velocity.y *= -1.0f;
    }

    this->position.x += this->velocity.x * deltaTime;
    this->position.y += this->velocity.y * deltaTime;

    // Regenerate vertices at the new position
    this->GenerateVertices();
}

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

        circleVertices.push_back({ centerVertexPos, centerVertexPos, this->color });
        circleVertices.push_back({ p1, p1, this->color });
        circleVertices.push_back({ p2, p2, this->color });
    }
}

std::vector<newVertex> Circle::GetCircleVertices()
{
    return this->circleVertices;
}

vec3 Circle::getPosition()
{
    return this->position;
}
