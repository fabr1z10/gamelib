#pragma once

#include "gamelib/component.h"
#include <glm/glm.hpp>

class ICollisionEngine;

struct RaycastOrigins {
	glm::vec2 topFwd, topBack;
	glm::vec2 bottomFwd, bottomBack;
};

struct CollisionDetails {
	CollisionDetails() {
		reset();
	}
	bool above, below;
	bool left, right;
	bool climbingSlope;
	bool descendingSlope;
	glm::vec3 velocityOld = glm::vec3(0.f);
	float slopeAngle, slopeAngleOld;
	void reset();
};

class Controller2D : public Component {
public:
	Controller2D(float width, float height, int maskUp, int maskDown, glm::vec2 anchor);

	void start() override;

	void move(glm::vec2 delta);

private:
	void descendSlope(glm::vec2& delta);
	void horizontalCollisions(glm::vec2& delta) ;
	void verticalCollisions(glm::vec2& delta);
	void climbSlope(glm::vec2& velocity, float slopeAngle);
protected:
	void updateRaycastOrigins();
	float _width;
	float _height;

	glm::vec2 _anchor;
	RaycastOrigins _raycastOrigins;
	glm::vec2 _localBottomBack;
	glm::vec2 _localBottomFwd;
	glm::vec2 _localTopBack;
	glm::vec2 _localTopFwd;
	ICollisionEngine* _collisionEngine;
	CollisionDetails _details;
	int _maskDown;
	int _maskUp;
	float _maxDescendAngle;
	float _maxClimbAngle;
	float _skinWidth;
	int _horizontalRayCount = 4;
	int _verticalRayCount = 4;
	float _horizontalRaySpacing;
	float _verticalRaySpacing;
};

