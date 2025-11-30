#include "gamelib/playercontroller2d.h"

#include <iostream>
#include "gamelib/collision.h"
#include "gamelib/game.h"
#include "gamelib/node.h"
#include "gamelib/model.h"

PlayerController2D::PlayerController2D(float width, float height, int maskUp, int maskDown, float maxSpeed,
	float jumpHeight, float timeToJumpApex, float accelerationTime, glm::vec2 anchor) : Controller2D(width, height, maskUp, maskDown, anchor),
	_maxSpeed(maxSpeed), _velocity(0.f), _accelerationTime(accelerationTime) {
	_window = Game::instance().getWindow();
	_jumpVelocity = (2.f * jumpHeight) / timeToJumpApex;
	_gravity = -(2.f * jumpHeight) / (timeToJumpApex * timeToJumpApex);
	_acceleration = _maxSpeed / _accelerationTime;
}

void PlayerController2D::update(double dt) {
	float dtf = (float)dt;
	auto left = glfwGetKey(_window, GLFW_KEY_LEFT) == GLFW_PRESS;
	auto right = glfwGetKey(_window, GLFW_KEY_RIGHT) == GLFW_PRESS;
	auto up = glfwGetKey(_window, _jumpKey) == GLFW_PRESS;
	if (left && right) {
		left = right = false;
	}



	if (_details.below) {
		if (up) {
			_velocity.y = _jumpVelocity;
		} else {
			_velocity.y = 0.f;
		}
	} else {
		// bump head
		if (_details.above) {
			_velocity.y = 0;
		}
	}

	glm::vec2 acceleration(0.f, _gravity);

	if (left || right) {
		acceleration.x = _acceleration;
	} else {
		if (fabs(_velocity.x) > 0.1f) {
			// friction
			acceleration.x = -signf(_velocity.x) * _acceleration;
		} else {
			acceleration.x = 0.f;
			_velocity.x = 0.f;
		}
	}

	_velocity += acceleration * dtf;

	if (left || right) {
		if (fabs(_velocity.x) > _maxSpeed) {
			_velocity.x = signf(_velocity.x) * _maxSpeed;
		}
	}

	auto delta = _velocity * dtf;
	//std::cout << "Moving by delta: " << delta.x << ", " << delta.y << std::endl;

	move(delta);
	if (auto* model = _node->getModel(); model != nullptr) {
		if (fabs(_velocity.x) > 1.0f) {
			model->setAnimation("walk");
		} else {
			model->setAnimation("idle");
		}
	}
}

int PlayerController2D::keyCallback(GLFWwindow *, int key, int scancode, int action, int mods) {
	// TODO HANDLE FLIP
	if (action == GLFW_PRESS) {
		if (key == GLFW_KEY_LEFT) {
			_node->setFlipX(true);
		} else if (key == GLFW_KEY_RIGHT) {
			_node->setFlipX(false);
		}
	}
}