#ifndef INPUT_HANDLER_H_
#define INPUT_HANDLER_H_

#include <bitset>
#include <iostream>
#include <algorithm>
#include <unordered_map>
#include <utility>
#include <GLFW/glfw3.h>
#include "console.h"

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

typedef short int KeyCode;

struct InputHandler {
	enum KeyType {
		KEY_SPACE = GLFW_KEY_SPACE,
		KEY_ENTER = GLFW_KEY_ENTER,
		KEY_W = GLFW_KEY_W,
		KEY_A = GLFW_KEY_A,
		KEY_S = GLFW_KEY_S,
		KEY_D = GLFW_KEY_D,
		KEY_C = GLFW_KEY_C,
		KEY_ESC = GLFW_KEY_ESCAPE
	};

	static InputHandler& instance() {
		static InputHandler inputHandler;

		return inputHandler;
	}

	bool isPress(KeyCode);
	bool isPress(KeyCode, KeyCode);
	bool isPress(KeyboardModifier, KeyCode);
	bool isPress(MouseModifier);

	void setKeyDown(KeyCode key);
	void setKeyUp(KeyCode key);
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
	bool isKeyPress(KeyCode);
	bool isModifierPress(KeyboardModifier);
	bool isModifierPress(MouseModifier);

	std::bitset<8> keyboardModifiers_;
	std::bitset<8> mouseModifiers_;
	std::unordered_map<KeyCode, bool> map_;
};

static void onKeyPress(GLFWwindow* window, int key, int scancode, int action, int mods);
static void onMouseClick(int button, int state, int x, int y);
static void onMouseMove(GLFWwindow* window, double x, double y);

#endif
