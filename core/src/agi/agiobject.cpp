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

AGICharacter::AGICharacter(const std::string& id, int x, int y, float speed, int direction) : AGIObject(id, x, y),
	_direction(direction), _speed(speed) {

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
	if (color == 0 || (_blocked.find(color) != _blocked.end())) {
		return false;
	}
	//if (color == 4) return true;
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

void AGICharacter::move() {
	if ((_direction & 0x0C) == 0) return;
	//if (_direction == 0) return;
	glm::vec3 pos = getWorldPosition();
	//std::cout << pos.x << ", " << pos.y << "\n";
	int px0 = (int) pos.x;
	int py0 = (int) pos.y;
	int dx = (_direction & 0x04) ? (_direction & 0x01 ? -1 : 1) : 0;
	int dy = (_direction & 0x08) ? (_direction & 0x02 ? -1 : 1) : 0;
	float speed = _speed;
	if (dx != 0 && dy != 0) {
		speed *= 0.707106f;
	}
	float vx = dx * speed;
	float vy = dy * speed;
	float x0 = pos.x;
	float y0 = pos.y;
	float x1 = x0 + vx;
	float y1 = y0 + vy;
	// start / end pixels
	int ix = (int)std::floor(x0);
	int iy = (int)std::floor(y0);
	int ixEnd = (int)std::floor(x1);
	int iyEnd = (int)std::floor(y1);

	// stepping direction
	int stepX = (vx > 0) - (vx < 0);		// it's either 1 or -1 or 0
	int stepY = (vy > 0) - (vy < 0);

	float tMaxX, tMaxY;
	float tDeltaX, tDeltaY;

	// setup DDA
	if (vx != 0.0f) {
		float nextX = (stepX > 0) ? (ix + 1.0f) : (float)ix;
		tMaxX = (nextX - x0) / vx;
		tDeltaX = 1.0f / std::abs(vx);
	} else {
		tMaxX = INFINITY;
		tDeltaX = INFINITY;
	}

	if (vy != 0.0f) {
		float nextY = (stepY > 0) ? (iy + 1.0f) : (float)iy;
		tMaxY = (nextY - y0) / vy;
		tDeltaY = 1.0f / std::abs(vy);
	} else {
		tMaxY = INFINITY;
		tDeltaY = INFINITY;
	}


	_moved = false;

	// traverse pixels
	while (ix != ixEnd || iy != iyEnd) {

		if (!checkPixel(ix, iy))
			return;   // movement blocked, stop entirely

		if (tMaxX < tMaxY) {
			tMaxX += tDeltaX;
			ix += stepX;
		} else {
			tMaxY += tDeltaY;
			iy += stepY;
		}
	}

	// check final pixel
	if (!checkPixel(ixEnd, iyEnd))
		return;

	for (const auto& h : _hotspots) {
		std::cout << "check " << x1 << "\n";
		h.check(this, (int) x1, (int) y1);
	}
	// movement successful
	this->setPosition(glm::vec3(x1, y1, 0.f));
	this->adjustPriority();
	_moved = true;}

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
	move();
	//std::cout << " Dir: " << (int)_direction << "\n";
	animate();
}

void AGINPC::customUpdate(double dt) {
	if (_suspendMovement) return;
	move();
	_direction = _strategy->getNextDirection(_direction, _moved);
	animate();
}

void AGINPC::setStrategy(std::shared_ptr<NPCStrategy> strategy) {
	_strategy = strategy;
}

void AGIObject::setPriorityCalculator(std::shared_ptr<PriorityCalculator> pc) {
	_priorityCalculator = pc;
	adjustPriority();
}

void AGIObject::setCallback(int id, agi::Callback callback) {
	_callbacks[id] = std::move(callback);
}

void AGIObject::setRectCallback(int x0, int x1, int y0, int y1, agi::Callback callback) {
	_hotspots.emplace_back(x0, x1, y0, y1, std::move(callback));
}

void AGIObject::addBlocked(int id) {
	_blocked.insert(id);
}