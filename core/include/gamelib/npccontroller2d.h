#pragma once

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include "gamelib/controller2d.h"
#include "gamelib/keylistener.h"

class NPCController2D : public Controller2D {
public:
	NPCController2D(float width, float height, int maskUp, int maskDown, float maxSpeed,
					   float jumpHeight, float timeToJumpApex, float accelerationTime, glm::vec2 anchor);

	void update(double) override;

private:
	float _maxSpeed;
	glm::vec2 _velocity;
	float _jumpVelocity;
	float _gravity;
	float _accelerationTime;
	float _acceleration;
};