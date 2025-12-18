#include "gamelib/agi/agiobject.h"
#include "gamelib/game.h"
#include "gamelib/model.h"
#include "gamelib/agi/agiroom.h"

using namespace agi;

AGIObject::AGIObject(int x, int y, float speed) : Node(), _direction(0), _speed(speed) {
	_room = dynamic_cast<agi::AGIRoom*>(Game::instance().getCurrentRoom());
	this->setPosition(glm::vec3(x, y, 0));
	//adjustPriority();
	_animationMap = { {0, ""}, {1, "0"}, {2, "1"}, {4, "3"}, {5, "0"}, {6, "1"}, {8, "2"}, {9, "0"}, {10, "1"} };

}


void AGIObject::adjustPriority() {
	glm::vec3 pos = this->_position;
	float z = _priorityCalculator->computeZ(this->_position);
	this->setPosition(glm::vec3(pos.x, pos.y, z));
}

void AGIObject::animate() {
	auto it = _animationMap.find(_direction);
	if (it != _animationMap.end()) {
		if (it->second.empty()) {
			_model->setUpdate(false);
		} else {
			_model->setUpdate(true);
			_model->setAnimation(it->second);
		}
		return;
	} else {

	}
}

void AGIObject::customUpdate(double) {
	auto* window = Game::instance().getWindow();
	auto upDown = glfwGetKey(window, GLFW_KEY_UP);
	auto downDown = glfwGetKey(window, GLFW_KEY_DOWN);
	auto leftDown = glfwGetKey(window, GLFW_KEY_LEFT);
	auto rightDown = glfwGetKey(window, GLFW_KEY_RIGHT);
	_direction = 0;
	if (upDown != downDown) {
		_direction |= upDown ? 4 : 8;
	}
	if (leftDown != rightDown) {
		_direction |= rightDown ? 1 : 2;
	}

	bool moved{false};

	glm::vec3 pos = getWorldPosition();
	if (_direction & 0x03) {
		pos.x += _speed * ((_direction & 0x01) ? 1 : -1);
		moved = true;
	}
	if (_direction & 0x0C) {
		pos.y += _speed * ((_direction & 0x04) ? 1 : -1);
		moved = true;
	}
	if (moved) {
		setPosition(pos);
		adjustPriority();
	}
	animate();
}

void AGIObject::setPriorityCalculator(std::shared_ptr<PriorityCalculator> pc) {
	_priorityCalculator = pc;
	adjustPriority();
}