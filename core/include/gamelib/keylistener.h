#pragma once

struct GLFWwindow;

class KeyListener {
public:
	KeyListener();

	virtual ~KeyListener();

	virtual int keyCallback(GLFWwindow*, int key, int scancode, int action, int mods) = 0;
};