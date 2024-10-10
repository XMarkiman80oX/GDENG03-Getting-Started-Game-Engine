#pragma once
#include <vector>
#include <DirectXMath.h>
#include "AppWindow.h"

struct newVertex 
{
	vec3 position;
	vec3 color;
	newVertex(vec3 position, vec3 color) 
	{
		this->position = position;
		this->color = color;
	}
};

class Circle
{
private:
	float radius;
	int segmentCount;
	vec3 centerPoint;
	std::vector<vertex> circleVertices;

public:
	Circle(int segmentCount, float radius, vec3 centerPoint, vec3 color);
	~Circle();

private:
	void GenerateVertices();

public:
	std::vector<vertex> GetCircleVertices();
};

