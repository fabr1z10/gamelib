#include "gamelib/npccontroller2d.h"

#include <iostream>
#include "gamelib/collision.h"
#include "gamelib/game.h"
#include "gamelib/node.h"
#include "gamelib/model.h"

NPCController2D::NPCController2D(float width, float height, int maskUp, int maskDown, float maxSpeed,
									   float jumpHeight, float timeToJumpApex, float accelerationTime, glm::vec2 anchor) : Controller2D(width, height, maskUp, maskDown, anchor),
																														   _maxSpeed(maxSpeed), _velocity(0.f), _accelerationTime(accelerationTime) {
	_jumpVelocity = (2.f * jumpHeight) / timeToJumpApex;
	_gravity = -(2.f * jumpHeight) / (timeToJumpApex * timeToJumpApex);
	_acceleration = _maxSpeed / _accelerationTime;
}

void NPCController2D::update(double dt) {
	float dtf = (float)dt;



	if (_details.below) {
		_velocity.y = 0.f;
	} else {
		// bump head
		if (_details.above) {
			_velocity.y = 0;
		}
	}

	glm::vec2 acceleration(0.f, _gravity);

	if (false) {
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

	if (false) {
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

