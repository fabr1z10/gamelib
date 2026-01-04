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