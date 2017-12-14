#ifndef INPUT_HANDLER_H_
#define INPUT_HANDLER_H_

#include <bitset>
#include <iostream>
#include <algorithm>
#include <unordered_map>
#include <utility>
#include <GLFW/glfw3.h>
#include "console.h"

#define KEY_SPACE 32
#define KEY_ENTER 257
#define KEY_W 87
#define KEY_A 65
#define KEY_S 83
#define KEY_D 68
#define KEY_C 67
#define KEY_ESC 256

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

	bool isPress(short);
	bool isPress(short, short);
	bool isPress(KeyboardModifier, short);
	bool isPress(MouseModifier);

	void setKeyDown(short key);
	void setKeyUp(short key);
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
	bool isKeyPress(short);
	bool isModifierPress(KeyboardModifier);
	bool isModifierPress(MouseModifier);

	// char modifiers_;
	std::bitset<8> keyboardModifiers_;
	std::bitset<8> mouseModifiers_;
	std::unordered_map<short, bool> map_;
};

static void onKeyPress(GLFWwindow* window, int key, int scancode, int action, int mods);
static void onMouseClick(int button, int state, int x, int y);
static void onMouseMove(GLFWwindow* window, double x, double y);

#endif
