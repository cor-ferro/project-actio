#include "input_handler.h"

InputHandler::InputHandler(WindowContext& windowContext)
	: keyboardModifiers_(0)
	, mouseModifiers_(0)
{
	windowContext.onKeyPress.connect([this](int key, int scancode, int action, int mods) {
		onKeyPress(key, scancode, action, mods);
	});

	windowContext.onMouseMove.connect([this](double x, double y) {
		onMouseMove(x, y);
	});
}

void InputHandler::onFrame()
{
	setMouseMovedPosition(mouseStart.x, mouseStart.y); // reset moved
}

bool InputHandler::isPress(KeyCode key)
{
	return isKeyPress(key);
}

bool InputHandler::isPress(KeyCode key1, KeyCode key2)
{
	return isKeyPress(key1) && isKeyPress(key2);
}

bool InputHandler::isPress(KeyboardModifier modifier, KeyCode key)
{
	return isModifierPress(modifier) && isKeyPress(key);
}

bool InputHandler::isPress(MouseModifier modifier)
{
	return isModifierPress(modifier); 
}

bool InputHandler::isKeyPress(KeyCode key)
{
	std::map<KeyCode, bool>::const_iterator it = map_.find(key);

	return it != map_.end() && it->second == true;
}

bool InputHandler::isModifierPress(KeyboardModifier modifier)
{
	return keyboardModifiers_.test(modifier);
}

bool InputHandler::isModifierPress(MouseModifier modifier)
{
	return mouseModifiers_.test(modifier);
}

void InputHandler::setKeyDown(KeyCode key)
{
	std::map<KeyCode, bool>::iterator it = map_.find(key);

	if (it != map_.end()) {
		it->second = true;
	} else {
		map_.insert({ key, true });
	}
}

void InputHandler::setKeyUp(KeyCode key)
{
	std::map<KeyCode, bool>::iterator it = map_.find(key);
	if (it != map_.end()) {
		it->second = false;
	}
}

void InputHandler::setModifierDown(KeyboardModifier modifier)
{
	keyboardModifiers_.set(modifier, true);
}

void InputHandler::setModifierUp(KeyboardModifier modifier)
{
	keyboardModifiers_.set(modifier, false);
}

void InputHandler::setModifierDown(MouseModifier modifier)
{
	mouseModifiers_.set(modifier, true);
}

void InputHandler::setModifierUp(MouseModifier modifier) 
{
	mouseModifiers_.set(modifier, false);
}

void InputHandler::setMousePosition(int x, int y)
{
	mouse.x = x;
	mouse.y = y;
}

void InputHandler::setMouseStartPosition(int x, int y)
{
	mouseStart.x = x;
	mouseStart.y = y;
}

void InputHandler::setMouseMovedPosition(int x, int y)
{
	mouseMoved.x = mouseStart.x - x;
	mouseMoved.y = mouseStart.y - y;

	mouseStart.x = x;
	mouseStart.y = y;
}

void InputHandler::onKeyPress(int key, int scancode, int action, int mods)
{
	switch (action) {
		case KEY_PRESS:
			setKeyDown(key);
			break;
		case KEY_RELEASE:
			setKeyUp(key);
			break;
	}
}

void InputHandler::onMouseMove(double x, double y)
{
	setMousePosition(x, y);
	setMouseMovedPosition(x, y);
}

void InputHandler::onMouseClick(int button, int state, int x, int y)
{
	MouseModifier mouseModifier;

	switch (button) {
	case MOUSE_BUTTON_LEFT:
		mouseModifier = MouseLeft;
		break;
	case MOUSE_BUTTON_MIDDLE:
		mouseModifier = MouseMiddle;
		break;
	case MOUSE_BUTTON_RIGHT:
		mouseModifier = MouseRight;
		break;
	}

	switch (state) {
	case KEY_PRESS:
		setModifierDown(mouseModifier);
		break;
	case KEY_RELEASE:
		setModifierUp(mouseModifier);
		break;
	}
}
