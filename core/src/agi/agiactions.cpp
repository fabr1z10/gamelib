#include "gamelib/agi/agiactions.h"
#include "gamelib/agi/agiroom.h"
#include "gamelib/agi/agiobject.h"
#include "gamelib/game.h"


using namespace agi;

SetModel::SetModel(const std::string &objectId, const std::string &modelId) : AGIAction(), _objectId(objectId), _modelId(modelId) {
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

void Print::start() {
	AGIAction::start();
	_room->print(_msgId);

}

void Print::run(double) {
	_completed=true;
}