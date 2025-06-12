#include "InputSystem.h"
#include <Windows.h>

InputSystem::InputSystem()
{
}

InputSystem::~InputSystem()
{
}

void InputSystem::addListener(InputListener* listener)
{
	this->mapListeners.insert(std::make_pair<InputListener*, InputListener*>
		(std::forward<InputListener*>(listener), std::forward<InputListener*>(listener)));
}

void InputSystem::removeListener(InputListener* listener)
{
	std::map<InputListener*, InputListener*>::iterator it = this->mapListeners.find(listener);

	if (it != this->mapListeners.end())
		this->mapListeners.erase(it);
}

void InputSystem::update()
{
	if (::GetKeyboardState(this->keysState)) {
		for (unsigned int i = 0; i < KEY_SIZE; i++) {
			//Only the high bits of the value is evaluated
			//if 1 or 2, KEY IS DOWN, otherwise NOT
			if (this->keysState[i] & 0x80) {
				this->processInput(i, KEY_STATE::KEY_DOWN);
			}
			//KEY IS UP
			else {
				//Check if the key is up if the old state of the key is not equal to the current one
				//If that is the case then we have a release event so in this case we can loop through our listeners and not define them off our key up event 
				if (this->keysState[i] != this->oldKeysState[i]) {
					this->processInput(i, KEY_STATE::KEY_UP);
				}
			}
		}
		//store current keys state to old keys state buffer
		::memcpy(this->oldKeysState, this->keysState, sizeof(unsigned char) * KEY_SIZE);
	}
}

InputSystem* InputSystem::getInstance()
{
	static InputSystem system;
	return &system;
}

void InputSystem::processInput(unsigned int processedKeyIndex, KEY_STATE keyState)
{
	std::map<InputListener*, InputListener*>::iterator it = this->mapListeners.begin();

	while (it != this->mapListeners.end()) {

		if (keyState == KEY_STATE::KEY_UP)
			it->second->onKeyUp(processedKeyIndex);
		else if (keyState == KEY_STATE::KEY_DOWN)
			it->second->onKeyDown(processedKeyIndex);

		++it;
	}
}
