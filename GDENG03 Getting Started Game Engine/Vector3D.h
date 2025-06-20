#pragma once
#include <iostream>

class Vector3D
{
public:
	Vector3D(float x, float y, float z): x(x),y(y),z(z) {

	}
	Vector3D(float uniformXYZ) : x(uniformXYZ), y(uniformXYZ), z(uniformXYZ) {

	}
	Vector3D(): x(0),y(0),z(0) {

	}
	Vector3D(const Vector3D& vector) : x(vector.x), y(vector.y), z(vector.z) {

	}

	~Vector3D() {

	}

	static Vector3D lerp(const Vector3D& start, const Vector3D& end, float delta) {
		Vector3D vector;
		vector.x = start.x*(1.0f-delta) + end.x * delta;
		vector.y = start.y*(1.0f-delta) + end.y * delta;
		vector.z = start.z*(1.0f-delta) + end.z * delta;

		return vector;
	}
	Vector3D operator * (float num) {
		return Vector3D(x * num, y * num, z * num);
	}
	Vector3D operator + (Vector3D other) {
		return Vector3D(x + other.x, y + other.y, z + other.z);
	}
	Vector3D& operator=(const Vector3D& other)
	{
		// Check for self-assignment
		if (this == &other) {
			return *this;
		}

		// Copy the member variables
		this->x = other.x;
		this->y = other.y;
		this->z = other.z;

		// Return a reference to the current object to allow for chaining
		return *this;
	}
	bool operator == (Vector3D other) {
		bool isEqual = true;

		if (other.x != this->x) isEqual = false;
		if (other.y != this->y) isEqual = false;
		if (other.z != this->z) isEqual = false;

		return isEqual;
	}
	void printVector() {
		std::cout << "(" << this->x << ", " << this->y << ", " << this->z << ")"<<std::endl;
	}
public:
	float x, y, z;
};