#include "gamelib/controller2d.h"
#include "gamelib/node.h"
#include "gamelib/collisionengine.h"
#include "gamelib/collision.h"
#include "gamelib/game.h"
#include "gamelib/shapes/rect.h"


void CollisionDetails::reset() {
	above = below = false;
	left = right = false;
	climbingSlope = false;
	descendingSlope = false;
	slopeAngle = 0.0f;
	slopeAngleOld = slopeAngle;

}

Controller2D::Controller2D(float width, float height, int maskUp, int maskDown, glm::vec2 anchor) :
		Component(), _width(width), _height(height), _anchor(anchor), _skinWidth(.015f), _maskUp(maskUp), _maskDown(maskDown) {
	float x0 = -anchor.x;
	float y0 = -anchor.y;
	float x1 = -anchor.x + width;
	float y1 = -anchor.y + height;
	_localBottomBack = glm::vec2(x0, y0);
	_localBottomFwd = glm::vec2(x1, y0);
	_localTopBack = glm::vec2(x0, y1);
	_localTopFwd = glm::vec2(x1, y1);
	_maxClimbAngle = deg2rad(80.f);
	_maxDescendAngle = deg2rad(80.f);

}

void Controller2D::start() {
	auto* room = Game::instance().getCurrentRoom();
	auto* dbgBatch = room->getBatch("debug-line");

	_collisionEngine = room->getCollisionEngine();
	if (_collisionEngine == nullptr) {
		throw std::runtime_error("Added a controller2D but no collision engine is set in the current room");
	}

	if (dbgBatch != nullptr) {
		auto shape = std::make_shared<shapes::Rect>(_width, _height, _anchor);
		auto model = shape->makeModel(dbgBatch, glm::vec4(1.f), ModelType::WIREFRAME);
		auto node = std::make_shared<Node>();
		node->setModel(model);
		_node->add(node);
	}
}


void Controller2D::move(glm::vec2 delta) {
	updateRaycastOrigins();

	bool wasGrounded = _details.below;
	_details.reset();
	//if (forced) {
	//		m_details.below = true;
	//}

	if (delta.y < 0 && wasGrounded) {
		descendSlope(delta);
	}

	if (!isEqual(delta.x, 0.0f)) {
		horizontalCollisions(delta);
	}
	if (!isEqual(delta.y, 0.0f)) {
		verticalCollisions(delta);
	}

	//m_node->move(glm::translate(delta));
	_node->move(delta);
}

void Controller2D::updateRaycastOrigins() {
	auto worldMatrix = _node->getWorldMatrix();

	_raycastOrigins.topFwd = worldMatrix * glm::vec4(_localTopFwd, 0.f, 1.0f);
	_raycastOrigins.topBack = worldMatrix * glm::vec4(_localTopBack, 0.f, 1.0f);
	_raycastOrigins.bottomFwd = worldMatrix * glm::vec4(_localBottomFwd, 0.f, 1.0f);
	_raycastOrigins.bottomBack = worldMatrix * glm::vec4(_localBottomBack, 0.f, 1.0f);
	_verticalRaySpacing = _width / (_verticalRayCount - 1);
	_horizontalRaySpacing = _height / (_horizontalRayCount - 1);
}

void Controller2D::descendSlope(glm::vec2& delta) {
	if (delta.x == 0.0f) return;

	bool goingForward = delta.x > 0.0f;
	auto directionX = (_node->lookRight() ? 1.f : -1.f);

	// find the bottom ray start point
	auto r0 = _raycastOrigins.bottomBack;
	auto raycastResult = _collisionEngine->raycastAxis(r0, -100.f, _maskDown, Axis::Y);
	if (raycastResult.hit) {
		float slopeAngle = angle(raycastResult.normal, glm::vec2(0.f, 1.f));
		if (slopeAngle != 0 && slopeAngle <= _maxDescendAngle) {
			if (signf(raycastResult.normal.x) == directionX) {
				// amount by which we go down the slope
				auto yDrop = fabs(delta.x) * tan(slopeAngle);
				if (raycastResult.distance - _skinWidth <= yDrop) {
					float moveDistance = fabs(delta.x);
					float descendDeltaY = moveDistance * sin(slopeAngle);
					float descendDeltaX = moveDistance * cos(slopeAngle);
					delta.x = directionX * descendDeltaX;// * sign(velocity.x);
					delta.y -= descendDeltaY;
					_details.slopeAngle = slopeAngle;
					_details.descendingSlope = true;
					_details.below = true;
				}
			}
		}
	}
}



void Controller2D::horizontalCollisions(glm::vec2& delta) {
	float directionX = signf(delta.x);
	bool goingForward = directionX > 0.f;

	float rayLength = fabs(delta.x) + _skinWidth;

	auto r0 = directionX > 0.f ? _raycastOrigins.bottomFwd : _raycastOrigins.bottomBack;
	// dir_x = 1 if we are moving to the right (i.e. look right & going forward or look left & going backward)
	float dir_x = (goingForward == _node->lookRight()) ? 1.f : -1.f;
	for (int i = 0; i < _horizontalRayCount; i++) {
		auto rayOrigin = r0 + glm::vec2(0.0f, i * _horizontalRaySpacing);
		auto hit = _collisionEngine->raycastAxis(rayOrigin, dir_x*rayLength, 2 | 32, Axis::X);
		if (hit.hit) {
			float slopeAngle = angle(hit.normal, glm::vec2(0.f, 1.f));
			if (i == 0 && slopeAngle <= _maxClimbAngle) {
				if (_details.descendingSlope) {
					_details.descendingSlope = false;
					delta = _details.velocityOld;
				}
				float distanceToSlopeStart = 0;
				if (slopeAngle != _details.slopeAngleOld) {
					distanceToSlopeStart = hit.distance - _skinWidth;
					delta.x -= distanceToSlopeStart * directionX;
				}
				climbSlope(delta, slopeAngle);
				delta.x += distanceToSlopeStart * directionX;
			}
			if (!_details.climbingSlope || slopeAngle > _maxClimbAngle) {
				delta.x = std::max(hit.distance - _skinWidth, 0.0f) * directionX;
				rayLength = hit.distance;
				if (_details.climbingSlope) {
					delta.y = tan(_details.slopeAngle)* fabs(delta.x);
				}
				bool faceRight = _node->lookRight();
				_details.left = dir_x == -1;
				_details.right = dir_x == 1;
			}
		}
	}
}


void Controller2D::climbSlope(glm::vec2& velocity, float slopeAngle) {
	float moveDistance = fabs(velocity.x);
	float climbVelocityY = sin(slopeAngle) * moveDistance;
	if (velocity.y <= climbVelocityY) {
		velocity.y = climbVelocityY;
		velocity.x = cos(slopeAngle) * moveDistance * signf(velocity.x);
		_details.below = true;
		_details.climbingSlope = true;
		_details.slopeAngle = slopeAngle;
	}
}


void Controller2D::verticalCollisions(glm::vec2& delta) {
	auto directionY = signf(delta.y);
	bool goingForward = delta.x > 0.0f;
	bool faceRight = _node->lookRight();

	float dir_x = (goingForward == faceRight) ? -1.f : 1.f;
	float rayLength = std::abs(delta.y) + _skinWidth;
	float obstacleDistance = std::numeric_limits<float>::max();

	Node* obstacle = nullptr;
	float dx = dir_x * fabs(delta.x);
	bool atleast = false;
	//glm::vec3 r0(m_raycastOrigins.xMin, 0.f, 0.f);
	glm::vec2 r0 = directionY > 0.f ?
			(faceRight ? _raycastOrigins.topBack : _raycastOrigins.topFwd) :
			(faceRight ? _raycastOrigins.bottomBack : _raycastOrigins.bottomFwd);
	for (int i = 0; i < _verticalRayCount; i++) {
		auto rayOrigin = r0 + glm::vec2(i * _verticalRaySpacing + dx, 0.f);
		int collMask = (directionY == -1 ? _maskDown : _maskUp);
		auto hit = _collisionEngine->raycastAxis(rayOrigin, directionY * rayLength, collMask, Axis::Y);
		//m_collisionEngine->rayCast(rayOrigin, glm::vec3(0.f, directionY, 0.f), rayLength, collMask);
		//bool ciao = m_details.below;
		if (hit.hit) {
			atleast = true;
			delta.y = (hit.distance - _skinWidth) * directionY;
			rayLength = hit.distance;
			if (_details.climbingSlope) {
				delta.x = (delta.y / tan(_details.slopeAngle)) * signf(delta.x);
			}
			_details.below = directionY == -1;
			_details.above = directionY == 1;
			if (directionY < 0.0f) {
				if (hit.distance < obstacleDistance) {
					//obstacle = hit.entity->getNode();
					obstacleDistance = hit.distance;
				}
				//obstacles.insert(hit.entity->getNode());
			}
		}
	}
}

