#pragma once

struct GLFWwindow;

class MouseListener {
public:
	MouseListener();

	virtual~ MouseListener();

	virtual void cursorPosCallback(GLFWwindow*, double, double) = 0;

	virtual int mouseButtonCallback(GLFWwindow*, int, int, int) = 0;
};