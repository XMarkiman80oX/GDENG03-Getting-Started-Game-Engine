#pragma once
#include "Window.h"
#include "iostream"
typedef struct vec3
{
	float x, y, z;

	vec3()
	{
		this->x = 1, this->y = 1, this->z = 1;
	}
	vec3(float x, float y, float z)
	{
		this->x = x, this->y = y, this->z = z;
	}
	vec3 operator+(const vec3& other) const
	{
		return vec3(x + other.x, y + other.y, z + other.z);
	}
	vec3& operator+=(const vec3& other) {
		x += other.x;
		y += other.y;
		z += other.z;
		return *this;
	}
	vec3 operator-(const vec3& other) const
	{
		return vec3(x - other.x, y - other.y, z - other.z);
	}
	vec3& operator-=(const vec3& other) {
		x -= other.x;
		y -= other.y;
		z -= other.z;
		return *this; // Return the current object (after modification)
	}
	bool operator==(const vec3& other) const
	{
		return x == other.x && y == other.y && z == other.z;
	}
} Vector3;

typedef struct vec2
{
	float x, y;

	vec2()
	{
		this->x = 1, this->y = 1;
	}
	vec2(float x, float y)
	{
		this->x = x, this->y = y;
	}
	vec2 operator+(const vec3& other) const
	{
		return vec2(x + other.x, y + other.y);
	}
	vec2 operator-(const vec3& other) const
	{
		return vec2(x - other.x, y - other.y);
	}
	bool operator==(const vec2& other) const
	{
		return x == other.x && y == other.y;
	}
} Vector2;

class Quad
{
protected:
	const int vertexCount = 4;
	const float length = 0.25;

private:
	Vector3* position;
	Vector3* colors;

public:
	Quad();
	~Quad();

public:
	//void setPosition(Vector2 position);
	void setPosition(float x, float y);
	void setColors(Vector3 color1, Vector3 color2 = Vector3(), Vector3 color3 = Vector3(), Vector3 color4 = Vector3());

public:
	int getVertexCount();
	Vector3* getVertexPositions();
	Vector3* getVertexColors();
};

