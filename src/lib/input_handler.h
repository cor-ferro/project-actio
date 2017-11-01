#ifndef INPUT_HANDLER_H_
#define INPUT_HANDLER_H_

#include <bitset>
#include <iostream>
#include <algorithm>
#include <unordered_map>
#include <utility>
#include <GL/glut.h>
#include "console.h"

#define KEY_SPACE 32
#define KEY_ENTER 13
#define KEY_W 119
#define KEY_A 97
#define KEY_S 115
#define KEY_D 100
#define KEY_ESC 27

#define MARGIN 30

enum KeyboardModifier {
	KeyboardShift = 1,
	KeyboardAlt = 2,
	KeyboardCtrl = 4,
	KeyboardMeta = 8,
};

enum MouseModifier {
	MouseLeft = 1,
	MouseMiddle = 2,
	MouseRight = 4,
};

struct MousePosition {
	int x;
	int y;
};

static int timerId;

struct InputHandler {
	static InputHandler& instance() {
		static InputHandler inputHandler;

		return inputHandler;
	}

	bool isPress(char);
	bool isPress(char, char);
	bool isPress(KeyboardModifier, char);
	bool isPress(MouseModifier);

	void setKeyDown(char key);
	void setKeyUp(char key);
	void setModifierDown(KeyboardModifier modifier);
	void setModifierUp(KeyboardModifier modifier);

	void setMouseDown(MouseModifier modifier);
	void setMouseUp(MouseModifier modifier);
	void setMousePosition(int x, int y);
	void setMouseMovedPosition(int x, int y);
	void setMouseStartPosition(int x, int y);
	void setModifierDown(MouseModifier modifier);
	void setModifierUp(MouseModifier modifier);

	void setWinSize(int width, int height);
	void pointerToCenter();

	void checkBoundings();
	void onFrame();

	MousePosition mouse;
	MousePosition mouseStart;
	MousePosition mouseMoved;

private:
	InputHandler();
//	~InputHandler();

	int winWidth;
	int winHeight;

	InputHandler(InputHandler const &);
	InputHandler& operator= (InputHandler const &);
	bool isKeyPress(char);
	bool isModifierPress(KeyboardModifier);
	bool isModifierPress(MouseModifier);

	// char modifiers_;
	std::bitset<8> keyboardModifiers_;
	std::bitset<8> mouseModifiers_;
	std::unordered_map<char, bool> map_;
};

static void onKeyPress(unsigned char key, int x, int y)
{
	InputHandler::instance().setKeyDown(key);
}

static void onKeyUp(unsigned char key, int x, int y)
{
	InputHandler::instance().setKeyUp(key);
}

static void onMouseClick(int button, int state, int x, int y)
{
	MouseModifier mouseModifier;

	switch (button) {
	case GLUT_LEFT_BUTTON:
		mouseModifier = MouseLeft;
		break;
	case GLUT_MIDDLE_BUTTON:
		mouseModifier = MouseMiddle;
		break;
	case GLUT_RIGHT_BUTTON:
		mouseModifier = MouseRight;
		break;
	}

	InputHandler& ih = InputHandler::instance();

	switch (state) {
	case GLUT_DOWN:
		ih.setModifierDown(mouseModifier);
		break;
	case GLUT_UP:
		ih.setModifierUp(mouseModifier);
		break;
	}
}

static void onMouseMove(int x, int y)
{
	InputHandler& ih = InputHandler::instance();

	ih.setMousePosition(x, y);
	ih.setMouseMovedPosition(x, y);
	ih.checkBoundings();
}

#endif
