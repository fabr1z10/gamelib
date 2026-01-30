#include "gamelib/agi/agiobject.h"
#include "gamelib/game.h"
#include "gamelib/model.h"
#include "gamelib/agi/agiroom.h"
#include "gamelib/algo/astar.h"
#include "gamelib/random.h"

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
	_direction(direction), _speed(speed), _segIndex(-1), _updateTime(1.0 / speed) {

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

void AGICharacter::move(double dt) {
	if ((_direction & 0x0C) == 0) return;
	_timeSinceLastUpdate += dt;
	int dx = (_direction & 0x04) ? (_direction & 0x01 ? -1 : 1) : 0;
	int dy = (_direction & 0x08) ? (_direction & 0x02 ? -1 : 1) : 0;
	double factor = (dx != 0 && dy != 0) ? 1.4142 : 1.0;
	if (_timeSinceLastUpdate < _updateTime * factor) return;
	_timeSinceLastUpdate = 0.0;
	glm::vec3 pos = getWorldPosition();
	float x0 = pos.x;
	float y0 = pos.y;
	int ix = (int)std::floor(x0);
	int iy = (int)std::floor(y0);
	int x1 = ix + dx;
	int y1 = iy + dy;
	// check final pixel
	_moved = false;
	if (!checkPixel(x1, y1)) {
		return;
	}

	if (dx != 0 && dy != 0) {
		// diagonal movement, check adjacent pixels too
		if (!(checkPixel(x1, iy) || checkPixel(ix, y1))) {
			return;
		}
	}

	for (const auto& h : _hotspots) {
		h.check(this, x1, y1);
	}
	// movement successful
	this->setPosition(glm::vec3(x1, y1, 0.f));
	if (_segIndex >= 0) {
		_currentLength += std::hypot(dx,dy);
	}
	this->adjustPriority();
	_moved = true;
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

void AGICharacter::walkTo(Point goal) {
	Point start {(int) this->getWorldPosition().x, (int) this->getWorldPosition().y};
	_segments.clear();
	std::vector<Point> path;
	astar<Point, PointHash>(_room->getGraph(), start, goal, path);
	// optimize path: find segments
	for (const auto& p : path) {
		std::cout << std::dec <<  p.x << ", " << p.y << "\n";
	}
	_path  = path;
	int i = 0;
	while (i+1 < path.size()) {
		int segStart = i;
		int hDir = path[i+1].x - path[i].x;
		int vDir = path[i+1].y - path[i].y;
		i++;
		while (path[i+1].x - path[i].x == hDir &&
				path[i+1].y - path[i].y == vDir &&
			   i < path.size()) {
			i++;
		}
		_segments.push_back({
			path[segStart],
			path[i],
			hDir,
			vDir,
			float(std::hypot(path[i].x - path[segStart].x, path[i].y - path[segStart].y))});
	}
	setWalkSegment(0);
}

void AGICharacter::setWalkSegment(int index) {
	const auto& s = _segments[index];
	_segIndex = index;
	_currentLength = 0.f;
	setPosition(glm::vec3(s.start.x, s.start.y, 0.f));
	adjustPriority();
	// set direction
	switch (s.hDir) {
		case 0:
			_direction &= 0xFB; // clear x movement (bit 2)
			break;
		case 1:
			_direction |= 0x04; // set bit 2
			_direction &= 0xFE; // face right		// clear bit 0
			break;
		case -1:
			_direction |= 0x05; // set bit 0 and bit 2
			break;
	}
	switch (s.vDir) {
		case 0:
			_direction &= 0xF7; // clear y movement (bit 3)
			break;
		case 1:
			_direction |= 0x08; // set right movement
			_direction &= 0xFD; // clear bit 1
			break;
		case -1:
			_direction |= 0x0A; // set bit 3 and bit 1
			break;
	}

}
void AGIPlayableCharacter::customUpdate(double dt) {
	if (_suspendMovement) return;
	if (_segIndex >= 0) {
		if (_currentLength >= _segments[_segIndex].length) {
			// move to next segment
			_currentLength = 0.f;
			_segIndex++;
			if (_segIndex >= _segments.size()) {
				_segIndex = -1;
				return;
			}
			setWalkSegment(_segIndex);
		}
	} else {
		// not walking on path, use keyboard
		_direction &= 0xF3; // clear movement bits
		auto* window = Game::instance().getWindow();
		auto upDown = glfwGetKey(window, GLFW_KEY_UP);
		auto downDown = glfwGetKey(window, GLFW_KEY_DOWN);
		auto leftDown = glfwGetKey(window, GLFW_KEY_LEFT);
		auto rightDown = glfwGetKey(window, GLFW_KEY_RIGHT);
		_direction |= (leftDown || rightDown) ? 0x04 : 0x00;
		_direction |= (upDown || downDown) ? 0x08 : 0x00;
	}







	move(dt);
	//std::cout << " Dir: " << (int)_direction << "\n";
	animate();
}

void AGINPC::customUpdate(double dt) {
	if (_suspendMovement) return;
	move(dt);
//	if (!_moved) {
//		auto pos = getWorldPosition();
//		auto ix = (int)std::floor(pos.x);
//		auto iy = (int)std::floor(pos.y);
//		std::vector<CharacterDirection> dirs { RIGHT, LEFT, UP, DOWN, UP_RIGHT, UP_LEFT, DOWN_RIGHT, DOWN_LEFT };
//
//		bool rightAllowed = checkPixel(ix+1, iy);
//		bool leftAllowed = checkPixel(ix-1, iy);
//		bool upAllowed = checkPixel(ix, iy+1);
//		bool downAllowed = checkPixel(ix, iy-1);
//		std::vector<bool> allowed{
//			rightAllowed,
//			leftAllowed,
//			upAllowed,
//			downAllowed,
//			checkPixel(ix+1, iy+1) && (rightAllowed || upAllowed),
//			checkPixel(ix-1, iy+1) && (leftAllowed || upAllowed),
//			checkPixel(ix+1, iy-1) && (rightAllowed || downAllowed),
//			checkPixel(ix-1, iy-1) && (leftAllowed || downAllowed)
//		};
//		std::vector<CharacterDirection> dd;
//		for (size_t i = 0; i < dirs.size(); i++) {
//			if (allowed[i]) dd.push_back(dirs[i]);
//		}
//
//		_direction = dd[Random::instance().nextInt(0, dd.size())];
//
//
//	}

	_direction = _strategy->getNextDirection(_direction, _moved);
	animate();
}

void AGINPC::setStrategy(std::shared_ptr<NPCStrategy> strategy) {
	_strategy = strategy;
	_strategy->setObject(this);
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