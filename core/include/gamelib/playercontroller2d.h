#pragma once

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include "gamelib/controller2d.h"
#include "gamelib/keylistener.h"

class PlayerController2D : public Controller2D, public KeyListener {
public:
	PlayerController2D(float width, float height, int maskUp, int maskDown, float maxSpeed,
					   float jumpHeight, float timeToJumpApex, float accelerationTime, glm::vec2 anchor);

	void update(double) override;

	int keyCallback(GLFWwindow*, int key, int scancode, int action, int mods) override;

private:
	GLFWwindow* _window;
	float _maxSpeed;
	int _jumpKey = GLFW_KEY_UP;
	glm::vec2 _velocity;
	float _jumpVelocity;
	float _gravity;
	float _accelerationTime;
	float _acceleration;
};