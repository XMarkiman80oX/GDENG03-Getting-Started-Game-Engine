#pragma once

class Point {
public:
	Point(int x, int y) : x(x), y(y) {

	}
	Point() : x(0), y(0) {

	}
	Point(const Point & point) : x(point.x), y(point.y) {

	}
	~Point() {

	}
public:
	int x = 0;
	int y = 0;
};