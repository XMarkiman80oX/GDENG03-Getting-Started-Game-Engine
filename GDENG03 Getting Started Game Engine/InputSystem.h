#pragma once
#include "InputListener.h"
#include <map>

enum KEY_STATE {
	KEY_DOWN,
	KEY_UP
};

#define KEY_SIZE (256)

class InputSystem
{
private:
	std::map<InputListener*, InputListener*> mapListeners;

	//We know exactly when the key is down but not the precise moment in which it's released
	//Therefore an approach can be to store all the key states after the full loop in a suitable array
	unsigned char keysState[KEY_SIZE] = {};
	unsigned char oldKeysState[KEY_SIZE] = {};

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
	void processInput(unsigned int processedKeyIndex, KEY_STATE keyState);
};

