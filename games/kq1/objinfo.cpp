#include "objinfo.h"
#include "gamelib/agi/agiobject.h"

using namespace  agi;

ObjInfo::ObjInfo(const std::string& name, const YAML::Node & node) : _name(name) {
	_room = node["room"].as<int>(-1);
	_x = node["x"].as<int>();
	_y = node["y"].as<int>();
	_sprite = node["view"].as<std::string>("");
	_anim = node["anim"].as<std::string>("");
}

// Define global operator<<
std::ostream& operator<<(std::ostream& os, const ObjInfo& obj) {
	os << obj._name << ": room = " << obj._room << " @ (" << obj._x << "," << obj._y << ")" << std::endl;
	return os;
}

std::shared_ptr<agi::AGIObject> DefaultObjInfo::makeNode() {
	auto obj = std::make_shared<AGIObject>(_name, _x, _y);
	return obj;
}