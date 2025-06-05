#pragma once
#include "Circle.h"

class CircleManager
{
private:
    int defaultSegmentCount;
    // float defaultRadius; // Will be passed per circle

    std::vector<Circle*> circles;
    std::vector<newVertex> allCircleVertices;

public:
    CircleManager();
    ~CircleManager();

public:
    static CircleManager* get();

public:
    int getCircleCount();
    std::vector<Circle*> getCircles();
    int getDefaultSegmentCount();
    std::vector<newVertex> getAllCircleVertices();

public:
    // Modified spawnCircle
    void spawnCircle(vec3 position, vec3 color, float screenAspectRatio, float radius);
    void releaseCircles();
    void popCircle();
    void randomizeNewPositions(); // This will now trigger regeneration of vertices
};