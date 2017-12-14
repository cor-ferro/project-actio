#include "input_handler.h"

InputHandler::InputHandler()
{
	map_ = std::unordered_map<short, bool>();
	keyboardModifiers_ = 0;
	mouseModifiers_ = 0;

	mouse.x = 0;
	mouse.y = 0;

	mouseStart.x = 0;
	mouseStart.y = 0;

	mouseMoved.x = 0; 
	mouseMoved.y = 0;

	winHeight = 0;
	winWidth = 0;

	GLFWwindow * window = glfwGetCurrentContext();
	glfwSetKeyCallback(window, onKeyPress);
	glfwSetCursorPosCallback(window, onMouseMove);

	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	// glfwSetInputMode(window, GLFW_STICKY_KEYS, 1);
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
	std::unordered_map<KeyCode, bool>::const_iterator it = map_.find(key);

	return it != map_.end() && it->second == true;
}

bool InputHandler::isModifierPress(KeyboardModifier modifier)
{
	// return modifiers_ & modifier > 0;
	return keyboardModifiers_.test(modifier);
}

bool InputHandler::isModifierPress(MouseModifier modifier)
{
	return mouseModifiers_.test(modifier);
}

void InputHandler::setKeyDown(KeyCode key)
{
	std::unordered_map<KeyCode, bool>::iterator it = map_.find(key);

	if (it != map_.end())
	{
		it->second = true;
	}
	else
	{
		std::pair<KeyCode, bool> keyInfo (key, true);
		map_.insert(keyInfo);
	}
}

void InputHandler::setKeyUp(KeyCode key)
{
	std::unordered_map<KeyCode, bool>::iterator it = map_.find(key);
	if (it != map_.end())
	{
		it->second = false;
	}
}

void InputHandler::setModifierDown(KeyboardModifier modifier)
{
	// modifiers_|= modifier;
	keyboardModifiers_.set(modifier, true);
}

void InputHandler::setModifierUp(KeyboardModifier modifier)
{
	// modifiers_^= modifier;
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

void InputHandler::setWinSize(int width, int height)
{
	winWidth = width;
	winHeight = height;

	console::info("window width ", winWidth);
	console::info("window height ", winHeight);
}

void InputHandler::pointerToCenter() {
	int x = winWidth / 2;
	int y = winHeight / 2;

	GLFWwindow * window = glfwGetCurrentContext();
	glfwSetCursorPos(window, x, y);
	setMousePosition(x, y);
	setMouseStartPosition(x, y);
	setMouseMovedPosition(x, y);
}

void InputHandler::checkBoundings()
{
	if (mouse.x < MARGIN || mouse.y < MARGIN) {
		pointerToCenter();
	}

	if ((winWidth - mouse.x < MARGIN) || (winHeight - mouse.y < MARGIN)) {
		pointerToCenter();
	}
}

void InputHandler::onFrame()
{
	setMouseMovedPosition(mouseStart.x, mouseStart.y); // reset moved
}


static void onKeyPress(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	if (action == GLFW_PRESS) {
		InputHandler::instance().setKeyDown(key);
	} else if (action == GLFW_RELEASE) {
		InputHandler::instance().setKeyUp(key);
	}
}

static void onMouseClick(int button, int state, int x, int y)
{
	MouseModifier mouseModifier;

	switch (button) {
	case GLFW_MOUSE_BUTTON_LEFT:
		mouseModifier = MouseLeft;
		break;
	case GLFW_MOUSE_BUTTON_MIDDLE:
		mouseModifier = MouseMiddle;
		break;
	case GLFW_MOUSE_BUTTON_RIGHT:
		mouseModifier = MouseRight;
		break;
	}

	InputHandler& ih = InputHandler::instance();

	switch (state) {
	case GLFW_PRESS:
		ih.setModifierDown(mouseModifier);
		break;
	case GLFW_RELEASE:
		ih.setModifierUp(mouseModifier);
		break;
	}
}

static void onMouseMove(GLFWwindow* window, double x, double y)
{
	InputHandler& ih = InputHandler::instance();

	ih.setMousePosition(x, y);
	ih.setMouseMovedPosition(x, y);
	// ih.checkBoundings();
}