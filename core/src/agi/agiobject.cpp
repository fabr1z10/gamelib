#include "gamelib/agi/agiobject.h"
#include "gamelib/game.h"
#include "gamelib/model.h"
#include "gamelib/agi/agiroom.h"

using namespace agi;

AGIObject::AGIObject(const std::string& id, int x, int y) : Node(), _id(id) {
	_room = dynamic_cast<agi::AGIRoom*>(Game::instance().getCurrentRoom());
	this->setPosition(glm::vec3(x, y, 0));
}

void AGIObject::reposition(int x, int y) {
	this->setPosition(glm::vec3(x, y, 0));
	adjustPriority();
}

AGICharacter::AGICharacter(const std::string& id, int x, int y, float speed) : AGIObject(id, x, y), _direction(0), _speed(speed) {

}




void AGIObject::adjustPriority() {
	glm::vec3 pos = this->_position;
	float z = _priorityCalculator->computeZ(this->_position);
	this->setPosition(glm::vec3(pos.x, pos.y, z));
}

void AGICharacter::animate() {
	if (_suspendMovement) return;
	if ((_direction & 0x0C) == 0) {
		_model->setUpdate(false);
		return;
	} else {
		auto it = _animationMap.find(_direction);
		if (it != _animationMap.end()) {
			_model->setUpdate(true);
			_model->setAnimation(it->second);
		}
	}
}

void AGICharacter::setModel(std::shared_ptr<IModel> model) {
	Node::setModel(model);
	bool hasLeft = model->hasAnimation("1");
	bool hasUp = model->hasAnimation("3");
	bool hasDown = model->hasAnimation("2");
	_animationMap = {
		{0x00, "0"},
		{0x01, hasLeft ? "1" : "0"},
		{0x02, "0"},
		{0x03, hasLeft ? "1" : "0"},
		{0x04, "0"},						// Moving right
		{0x05, hasLeft ? "1" : "0"},		// Moving left
		{0x06, "0"},						// Moving right
		{0x07, hasLeft ? "1" : "0"},		// Moving left
		{0x08, hasUp ? "3" : "0"},			// Moving up, last facing right
		{0x09, hasUp ? "3" : hasLeft ? "1": "0"},			// Moving up, last facing left
		{0x0A, hasDown ? "2" : "0"},
		{0x0B, hasDown ? "2" : hasLeft ? "1": "0"},		// Moving down, last facing left
		{0x0C, "0"},						// Moving up & right
		{0x0D, hasLeft ? "1" : "0"},		// Moving up & left
		{0x0E, "0"},						// Moving down & right
		{0x0F, hasLeft ? "1" : "0"},		// Moving down & left
	};
	auto it = _animationMap.find(_direction);
	if (it != _animationMap.end()) {
		_model->setUpdate(true);
		_model->setAnimation(it->second);
	}

}

bool AGIObject::checkPixel(int x, int y) {
	int color = _room->test(x, y);
	if (color == 4) return true;
	if (color == 0) return false;
	auto cb = _callbacks.find(color);
	if (cb != _callbacks.end()) {
		int canGo = cb->second(this, x, y);
		if (canGo != 0) return false;
	}
	return true;

}

int AGICharacter::has(const std::string &item) const {
	auto it = _inventory.find(item);
	if (it != _inventory.end()) {
		return it->second;
	} else {
		return 0;
	}
}

void AGICharacter::addToInventory(const std::string item, int qty) {
	if (qty == 0) {
		_inventory.erase(item);
		return;
	}
	_inventory[item] = qty;
}

void AGICharacter::move(int delta) {
	if ((_direction & 0x0C) == 0) return;
	//if (_direction == 0) return;
	glm::vec3 pos = getWorldPosition();
	//std::cout << pos.x << ", " << pos.y << "\n";
	int x0 = (int) pos.x;
	int y0 = (int) pos.y;
	int dx = (_direction & 0x04) ? (_direction & 0x01 ? -1 : 1) : 0;
	int dy = (_direction & 0x08) ? (_direction & 0x02 ? -1 : 1) : 0;
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
	if (moved && !_suspendMovement) {
		pos = glm::vec3(cx, cy, pos.z);
		setPosition(pos);
		adjustPriority();
	}
}

int AGIPlayableCharacter::keyCallback(GLFWwindow *, int key, int scancode, int action, int mods) {
	if (_suspendMovement) return 0;

	if (action== GLFW_PRESS) {
		if (key == GLFW_KEY_RIGHT) {
			_direction &= 0xFE;		/// set LSB to 0
		} else if (key == GLFW_KEY_LEFT) {
			_direction |= 0x01;		/// set LSB to 1
		} else if (key == GLFW_KEY_UP) {
			_direction &= 0xFD;		/// set bit 1 to 0
		} else if (key == GLFW_KEY_DOWN) {
			_direction |= 0x02;		/// set bit 1 to 1
		}
	}
}
void AGIPlayableCharacter::customUpdate(double) {
	if (_suspendMovement) return;
	_direction &= 0xF3; // clear movement bits
	auto* window = Game::instance().getWindow();
	auto upDown = glfwGetKey(window, GLFW_KEY_UP);
	auto downDown = glfwGetKey(window, GLFW_KEY_DOWN);
	auto leftDown = glfwGetKey(window, GLFW_KEY_LEFT);
	auto rightDown = glfwGetKey(window, GLFW_KEY_RIGHT);
	_direction |= (leftDown || rightDown) ? 0x04 : 0x00;
	_direction |= (upDown || downDown) ? 0x08 : 0x00;
	move(_speed);
	std::cout << " Dir: " << (int)_direction << "\n";
	animate();
}

void AGIObject::setPriorityCalculator(std::shared_ptr<PriorityCalculator> pc) {
	_priorityCalculator = pc;
	adjustPriority();
}

void AGIObject::setCallback(int id, Callback callback) {
	_callbacks[id] = callback;

}