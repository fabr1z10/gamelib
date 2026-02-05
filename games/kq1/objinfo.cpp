#include "objinfo.h"
#include "gamelib/agi/agiobject.h"
#include "gamelib/agi/agiroom.h"
#include "gamelib/agi/agistrategy.h"

using namespace  agi;

ObjInfo::ObjInfo(const std::string& name, const YAML::Node & node) : _name(name) {
	_room = node["room"].as<int>(-1);
	_x = node["x"].as<int>();
	_y = node["y"].as<int>();
	_sprite = node["view"].as<std::string>("");
	_anim = node["anim"].as<std::string>("");
	_noWalkColors = node["blocks"].as<std::vector<int>>(std::vector<int>{});
}

CharacterInfo::CharacterInfo(const std::string &name, const YAML::Node & node) : ObjInfo(name, node) {
	_direction = node["direction"].as<int>(0);
	_speed = node["speed"].as<int>(1);
	if (node["inventory"]) {
		for (const auto& item : node["inventory"]) {
			auto it = item.first.as<std::string>();
			auto qty = item.second.as<int>();
			_inventory[it] = qty;
		}
	}

}

PlayerInfo::PlayerInfo(const std::string &name, const YAML::Node & node) : CharacterInfo(name, node) {}

NPCInfo::NPCInfo(const std::string &name, const YAML::Node & node) : CharacterInfo(name, node) {
	_strategy = node["strategy"].as<std::string>("");
}



// Define global operator<<
std::ostream& operator<<(std::ostream& os, const ObjInfo& obj) {
	os << obj._name << ": room = " << obj._room << " @ (" << obj._x << "," << obj._y << ")" << std::endl;
	return os;
}

void ObjInfo::applyModel(std::shared_ptr<agi::AGIObject> obj, agi::AGIRoom * room) {
	if (!_sprite.empty()) {
		auto model = room->getModel(_sprite);
		obj->setModel(model);
	}
	for (const auto& b : _noWalkColors) {
		obj->addBlocked(b);
	}

}

std::shared_ptr<agi::AGIObject> DefaultObjInfo::makeNode(agi::AGIRoom* room) {
	auto obj = std::make_shared<AGIObject>(_name, _x, _y);
	applyModel(obj, room);

	return obj;
}

std::shared_ptr<agi::AGIObject> PlayerInfo::makeNode(agi::AGIRoom * room) {
	auto player = std::make_shared<agi::AGIPlayableCharacter>(_name, _x, _y, _speed, _direction);
	applyModel(player, room);
	return player;
}

std::shared_ptr<agi::AGIObject> NPCInfo::makeNode(agi::AGIRoom * room) {
	auto npc = std::make_shared<agi::AGINPC>(_name, _x, _y, _speed, _direction);
	applyModel(npc, room);
	if (_strategy == "wander") {
		auto strat = std::make_shared<agi::Wander>();
		npc->setStrategy(strat);
	}
	return npc;
}
