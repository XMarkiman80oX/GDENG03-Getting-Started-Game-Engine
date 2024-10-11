#pragma once
#include <vector>
#include <DirectXMath.h>
#include <iostream>
//#include "AppWindow.h"

struct vec3
{
	float x, y, z;

	vec3() {
		this->x = 0.0f; this->y = 0.0f; this->z = 0.0f;
	}
	vec3(float x, float y, float z) {
		this->x = x; this->y = y; this->z = z;
	}
};
struct vertex
{
	vec3 position;
	vec3 position1;
	vec3 color;
	vec3 color1;
};
struct newVertex 
{
	vec3 position;
	vec3 color;
	newVertex(vec3 position, vec3 color) 
	{
		this->position = position;
		this->color = color;
	}
	newVertex()
	{
		this->position = { 0,0,0 };
		this->color = { 0,0,0 };
	}
};
class AppWindow;
class Circle
{
private:
	vec3 color;
	float screenAspectRatio;
	vec3 position;
	float radius;
	int segmentCount;
	std::vector<newVertex> circleVertices;

public:
	Circle(int segmentCount, float radius, vec3 position, vec3 color, float screenAspectRatio);
	~Circle();

private:
	void GenerateVertices();

public:
	std::vector<newVertex> GetCircleVertices();
};

