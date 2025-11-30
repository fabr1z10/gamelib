#include "gamelib/follow.h"
#include "gamelib/camera.h"
#include "gamelib/game.h"
#include "gamelib/node.h"

Follow::Follow (const std::string& camId, glm::vec3 relativePos ) : Component(), _camId(camId), _relativePos(relativePos) {
	_up = glm::vec3(0,1,0);
	_previous = glm::vec3(0.0f);
}

void Follow::start() {
	_cam = Game::instance().getCurrentRoom()->getCamera(_camId);
	auto worldPos = _node->getWorldTransform();
	glm::vec3 eye = worldPos.position + _relativePos;
	glm::vec3 dir(0.f, 0.f, -1.f);//(glm::normalize(m_node->getWorldPosition() - eye);
	_cam->setPosition(eye, dir, _up);
	_previous = worldPos.position;
}


void Follow::update (double dt) {
	auto worldPos = _node->getWorldTransform().position;
	if (_previous - worldPos != glm::vec3(0.0f)) {
		glm::vec3 eye = worldPos + _relativePos;
		auto dir = glm::vec3(0.f, 0.f, -1.f);//(glm::normalize(m_node->getWorldPosition() - eye);
		_cam->setPosition(eye, dir, _up);
		_previous = worldPos;
	}

}