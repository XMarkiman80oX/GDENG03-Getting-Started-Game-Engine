#pragma once
#include "Point.h"
//INTERFACE
class InputListener {
public:
	InputListener() {

	}
	~InputListener() {

	}

	//FOR KB
	virtual void onKeyDown(int key) = 0;
	virtual void onKeyUp(int key) = 0;

	//FOR MOUSE
	//delta is the position or variation of the position of mouse cursor
	virtual void onMouseMove(const Point& mousePosition) = 0;
	
	virtual void onLeftMouseDown(const Point& mousePosition) = 0;
	virtual void onLeftMouseUp(const Point& mousePosition) = 0;

	virtual void onRightMouseDown(const Point& mousePosition) = 0;
	virtual void onRightMouseUp(const Point& mousePosition) = 0;
};