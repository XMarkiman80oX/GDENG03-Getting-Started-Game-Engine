#pragma once

class Vector2D
{
public:
	Vector2D(float x, float y) : x(x), y(y) {

	}
	Vector2D(float uniformXY) : x(uniformXY), y(uniformXY) {

	}
	Vector2D() : x(0), y(0) {

	}
	Vector2D(const Vector2D& vector) : x(vector.x), y(vector.y) {

	}

	~Vector2D() {

	}

	static Vector2D lerp(const Vector2D& start, const Vector2D& end, float delta) {
		Vector2D vector;
		vector.x = start.x * (1.0f - delta) + end.x * delta;
		vector.y = start.y * (1.0f - delta) + end.y * delta;

		return vector;
	}
	Vector2D operator * (float num) {
		return Vector2D(x * num, y * num);
	}
	Vector2D operator + (Vector2D vec) {
		return Vector2D(x + vec.x, y + vec.y);
	}
public:
	float x, y;
};