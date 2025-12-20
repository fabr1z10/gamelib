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

bool AGIObject::checkPixel(int x, int y) {
	int color = _room->test(x, y);
	if (color == 4) return true;
	if (color == 0) return false;
	auto cb = _callbacks.find(color);
	if (cb != _callbacks.end()) {
		int canGo = cb->second(x, y);
		if (canGo != 0) return false;
	}
	return true;

}

void AGIObject::move(int delta) {
	if (_direction == 0) return;
	glm::vec3 pos = getWorldPosition();
	std::cout << pos.x << ", " << pos.y << "\n";
	int x0 = (int) pos.x;
	int y0 = (int) pos.y;
	int dx = _direction & 0x01 ? 1 : (_direction & 0x02 ? -1 : 0);
	int dy = _direction & 0x04 ? 1 : (_direction & 0x08 ? -1 : 0);
	int i = 0;
	bool moved= false;
	bool allowX = (dx != 0);
	bool allowY = (dy != 0);
	int cx = x0;
	int cy = y0;
	for (i = 0; i < delta; ++i) {

//		if (allowX && allowY) {
//			if (!checkPixel(cx + dx, cy + dy)) {
//				allowX = allowY = false;
//				break;
//			}
//		}
		if (allowX) {
			if (checkPixel(cx + dx, cy)) {
				cx += dx;
				moved = true;
 			} else {
				allowX = false;
			}
		}
		if (allowY) {
			if (checkPixel(cx, cy + dy)) {
				cy += dy;
				moved = true;
			} else {
				allowY = false;
			}
		}
	}
	if (moved) {
		pos = glm::vec3(cx, cy, pos.z);
		setPosition(pos);
		adjustPriority();
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

	//bool moved{false};

	move(_speed);
//	glm::vec3 pos = getWorldPosition();
//	if (_direction & 0x03) {
//		// before moving, check control image!
//		auto nx = (int) (pos.x + _speed * ((_direction & 0x01) ? 1 : -1));
//		auto ny = (int) pos.y;
//		int color = _room->test(nx, ny);
//
//		std::cout << "Testing move to (" << nx << "," << ny << ") = color " << color << std::endl;
//
//		pos.x += _speed * ((_direction & 0x01) ? 1 : -1);
//		moved = true;
//	}
//	if (_direction & 0x0C) {
//		auto nx = (int) pos.x;
//		auto ny = (int) pos.y +  _speed * ((_direction & 0x04) ? 1 : -1);
//		int color = _room->test(nx, ny);
//		std::cout << "Testing move to (" << nx << "," << ny << ") = color " << color << std::endl;
//		pos.y += _speed * ((_direction & 0x04) ? 1 : -1);
//		moved = true;
//	}
	animate();
}

void AGIObject::setPriorityCalculator(std::shared_ptr<PriorityCalculator> pc) {
	_priorityCalculator = pc;
	adjustPriority();
}

void AGIObject::setCallback(int id, Callback callback) {
	_callbacks[id] = callback;

}