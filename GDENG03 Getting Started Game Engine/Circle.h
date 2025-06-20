#pragma once
#include <vector>
#include <DirectXMath.h>
#include <iostream>
#include <random>

struct vec3
{
	float x, y, z;

	vec3() 
	{
		this->x = 0.0f; this->y = 0.0f; this->z = 0.0f;
	}
	vec3(float x, float y, float z) 
	{
		this->x = x; this->y = y; this->z = z;
	}
	void randomizeVector(bool clampToEnds) 
	{
		std::random_device rd; 
		std::mt19937 eng(rd()); 

		std::uniform_real_distribution<> distrFloat(-1.0f, 1.0f); 

		this->x = distrFloat(eng);
		this->y = distrFloat(eng);
		this->z = 0;

		if (clampToEnds) {
			this->clampToEnds();
		}
	}
	void randomizeVector(bool clampToEnds, float lowerBound, float upperBound)
	{
		std::random_device rd;
		std::mt19937 eng(rd());

		std::uniform_real_distribution<> distrFloat(lowerBound, upperBound);

		this->x = distrFloat(eng);
		this->y = distrFloat(eng);
		this->z = 0;

		if (clampToEnds) {
			this->clampToEnds();
		}
	}
	void clampToEnds() {
		float abs_x = std::abs(this->x), abs_y = std::abs(this->y), abs_z = std::abs(this->z);
		float highest = 0;

		if (abs_x >= abs_y)
			highest = abs_x;
		else
			highest = abs_y;

		if (highest == abs_x) {
			if (this->x < 0) this->x = -1;
			else this->x = 1;
		}
		else if (highest == abs_y) {
			if (this->y < 0) this->y = -1;
			else this->y = 1;
		}
	}
	vec3 operator+(const vec3& other) const {
		return vec3(x + other.x, y + other.y, z + other.z);
	}
	vec3 operator-(const vec3& other) const {
		return vec3(x - other.x, y - other.y, z - other.z);
	}
	void printVector() 
	{
		std::cout << "x: " << this->x << " ,y: " << this->y << " ,z: " << this->z <<std::endl;
	}
};
struct newVertex 
{
	vec3 position;
	vec3 newPositionDistance;
	vec3 color;
	newVertex(vec3 position, vec3 color) 
	{
		this->position = position;
		this->newPositionDistance = vec3();
		this->color = color;
	}
	newVertex(vec3 position, vec3 newPositionDistance, vec3 color)
	{
		this->position = position;
		this->newPositionDistance = newPositionDistance;
		this->color = color;
	}
	newVertex()
	{
		this->position = vec3();
		this->newPositionDistance = vec3();
		this->color = vec3();
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

	vec3 velocity; 

public:
	Circle(int segmentCount, float radius, vec3 position, vec3 color, float screenAspectRatio);
	~Circle();

	void GenerateVertices();
	void initializeVelocity(); 
	void setScreenAspectRatio(float newAspectRatio);
	void update(float deltaTime);

public:
	std::vector<newVertex> GetCircleVertices();
	vec3 getPosition();
};

