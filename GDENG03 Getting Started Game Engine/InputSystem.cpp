#include "InputSystem.h"

InputSystem::InputSystem()
{
}

InputSystem::~InputSystem()
{
}

void InputSystem::addListener(InputListener* listener)
{
	this->setListeners.insert(std::make_pair<InputListener*, InputListener*>
		(std::forward<InputListener*>(listener), std::forward<InputListener*>(listener)));
}

void InputSystem::removeListener(InputListener* listener)
{
	std::map<InputListener*, InputListener*>::iterator it = this->setListeners.find(listener);

	if (it != this->setListeners.end())
		this->setListeners.erase(it);
}

void InputSystem::setCursorPosition(const Point& pos)
{
	::SetCursorPos(pos.x, pos.y);
}

void InputSystem::showCursor(bool show)
{
	::ShowCursor(show);
}

void InputSystem::update()
{
	this->processMouseInput();
	this->processKeyboardInput();
}

InputSystem* InputSystem::getInstance()
{
	static InputSystem system;
	return &system;
}

void InputSystem::processKeyboardInput()
{
	if (::GetKeyboardState(this->keysState)) {
		for (unsigned int i = 0; i < KEY_SIZE; i++) {
			//Only the high bits of the value is evaluated
			//if 1 or 2, KEY IS DOWN, otherwise NOT
			if (this->keysState[i] & 0x80) {
				this->processInputType(i, INPUT_STATE::KEY_DOWN);
			}
			//KEY IS UP
			else {
				//Check if the key is up if the old state of the key is not equal to the current one
				//If that is the case then we have a release event so in this case we can loop through our listeners and not define them off our key up event 
				if (this->keysState[i] != this->oldKeysState[i]) {
					this->processInputType(i, INPUT_STATE::KEY_UP);
				}
			}
		}
		//store current keys state to old keys state buffer
		::memcpy(this->oldKeysState, this->keysState, sizeof(unsigned char) * KEY_SIZE);
	}
}

void InputSystem::processMouseInput()
{
	POINT currentMousePosition = this->getCurrentMousePosition();

	if (this->isFirstTime) {
		this->oldMousePosition = Point(currentMousePosition.x, currentMousePosition.y);
		this->isFirstTime = false;
	}

	//Check if there's any change to the previous position of cursor respect to the previous update code
	if (currentMousePosition.x != oldMousePosition.x || currentMousePosition.y != oldMousePosition.y) {
		//THERE IS MOUSE MOVE EVENT
		//Therefore, we have to notify to all the listneres to this particular event
		this->processMouseMovement(Point(currentMousePosition.x, currentMousePosition.y));
	}
	//so that during the next update call we have a true and valid position
	this->oldMousePosition = Point(currentMousePosition.x, currentMousePosition.y);
}

void InputSystem::processInputType(unsigned int processedKeyIndex, INPUT_STATE keyState, const Point& pointData)
{
	std::map<InputListener*, InputListener*>::iterator it = this->setListeners.begin();
	
	//If it so happens to be a click
	POINT currentMousePos = {};
	currentMousePos = this->getCurrentMousePosition();

	while (it != this->setListeners.end()) {

		switch (keyState) {
			case KEY_DOWN:
				//If it so happens to be a click
				switch (processedKeyIndex)
				{//Event only needs to be called one time and not each time the update method is called
					//Checking if the previous state of the key is different respect to the current one, if so then trigger mouse down
					case VK_LBUTTON:
						it->second->onLeftMouseDown(Point(currentMousePos.x, currentMousePos.y));
						break;
					case VK_RBUTTON:
						it->second->onRightMouseDown(Point(currentMousePos.x, currentMousePos.y));
						break;
					default:
						it->second->onKeyDown(processedKeyIndex);
						break;
				}
				break;
			case KEY_UP:
				switch (processedKeyIndex)
				{//Event only needs to be called one time and not each time the update method is called
					//Checking if the previous state of the key is different respect to the current one, if so then trigger mouse down
					case VK_LBUTTON:
						it->second->onLeftMouseUp(Point(currentMousePos.x, currentMousePos.y));
						break;
					case VK_RBUTTON:
						it->second->onRightMouseUp(Point(currentMousePos.x, currentMousePos.y));
						break;
					default:
						it->second->onKeyUp(processedKeyIndex);
						break;
				}
				break;
		}

		++it;
	}
}

void InputSystem::processMouseMovement(const Point& pointData)
{
	std::map<InputListener*, InputListener*>::iterator it = this->setListeners.begin();

	while (it != this->setListeners.end()) {
		it->second->onMouseMove(pointData);
		++it;
	}
}

POINT InputSystem::getCurrentMousePosition()
{
	POINT currentMousePosition = {};
	::GetCursorPos(&currentMousePosition);

	return currentMousePosition;
}
