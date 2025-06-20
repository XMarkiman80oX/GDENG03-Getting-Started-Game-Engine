#pragma once
#include "Circle.h"

class CircleManager
{
private:
    int defaultSegmentCount;

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
    void spawnCircle(vec3 position, vec3 color, float screenAspectRatio, float radius);
    void releaseCircles();
    void popCircle();
    void rebuildAllCircleVertices();
};