#include "gamelib/agi/agiactions.h"
#include "gamelib/agi/agiroom.h"
#include "gamelib/agi/agiobject.h"
#include "gamelib/game.h"
#include "gamelib/model.h"

using namespace agi;

SetModel::SetModel(const std::string &objectId, const std::string &modelId) : AGIAction(), _objectId(objectId), _modelId(modelId) {
}

Animate::Animate(const std::string &objectId, const std::string &animationId, bool wait) :
	AGIAction(), _objectId(objectId), _animationId(animationId), _wait(wait), _model(nullptr) {
}

Print::Print(const std::string &message) : AGIAction(), _msgId(message) {}

void AGIAction::start() {
	_room = dynamic_cast<agi::AGIRoom*>(Game::instance().getCurrentRoom());

}

void SetModel::start() {
	AGIAction::start();
	_room->getObject(_objectId)->setModel(_room->getModel(_modelId));
	_completed = true;
}

void Animate::start() {
	AGIAction::start();
	_model = _room->getObject(_objectId)->getModel();
	_model->setAnimation(_animationId);
	if (!_wait) {
		_completed = true;
	}
}

void Animate::run(double) {
	if (_wait && _model->getLoopCount() > 0) {
		_completed = true;
	}
}


void Print::start() {
	AGIAction::start();
	_room->print(_msgId);

}

void Print::run(double) {
	_completed=true;
}


Walk::Walk(const std::string &objectId, const std::vector<Point> path) : AGIAction(), _objectId(objectId), _path(path) {
}

void Walk::start() {
	AGIAction::start();
	_object = dynamic_cast<AGICharacter*>(_room->getObject(_objectId));
	_object->setPosition(glm::vec3(_path[0].x, _path[0].y, 0));
	_index = 0;
}

void Walk::run(double) {
	_index++;
	if (_index >= _path.size()) {
		_completed=true;
		return;
	}
	if (_index + 1 < _path.size()) {
		auto curr = _path[_index];
		auto next = _path[_index + 1];
		uint8_t direction;
		if (next.x != curr.x) {
			if (next.x > curr.x) {
				// moving right
				direction = 0x04;
			} else {
				// moving left
				direction = 0x05;
			}
		}
		if (next.y != curr.y) {
			if (next.y > curr.y) {
				// moving down
				direction |= 0x0B;
			} else {
				// moving up
				direction |= 0x09;
			}
		}
		_object->setDirection(direction);
	}

	_object->setPosition(glm::vec3(_path[_index].x, _path[_index].y, 0));
	_object->adjustPriority();
	_object->animate();
}