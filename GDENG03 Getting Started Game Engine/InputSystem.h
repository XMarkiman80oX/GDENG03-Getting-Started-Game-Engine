#pragma once
#include "InputListener.h"
#include <map>
#include "Point.h"
#include "Windows.h"

enum INPUT_STATE {
	NONE = -1,
	KEY_DOWN,
	KEY_UP,
	MOUSE_MOVE
};
#define KEY_SIZE (256)

class InputSystem
{
private:
	std::map<InputListener*, InputListener*> setListeners;

	//We know exactly when the key is down but not the precise moment in which it's released
	//Therefore an approach can be to store all the key states after the full loop in a suitable array
	unsigned char keysState[KEY_SIZE] = {};
	unsigned char oldKeysState[KEY_SIZE] = {};

	//We need the prev position of cursor
	Point oldMousePosition;
	//"We have to check the current mouse position with a valid old mouse position"
	bool isFirstTime = true;

public:
	InputSystem();
	~InputSystem();
public:
	void addListener(InputListener* listener);
	void removeListener(InputListener* listener);
public:
	void update();

public:
	static InputSystem* getInstance();
private:
	void processKeyboardInput();
	void processMouseInput();
	void processInputType(unsigned int processedKeyIndex = 0, INPUT_STATE keyState = INPUT_STATE::NONE, const Point& pointData = Point());
	void processMouseMovement(const Point& pointData = Point());
	
	POINT getCurrentMousePosition();
};

