#pragma once
#include <string>
#include <gamelib/node.h>
#include <yaml-cpp/yaml.h>
#include <gamelib/agi/agiobject.h>

namespace agi {
	class AGIRoom;
}

class ObjInfo {

public:
	explicit ObjInfo(const std::string& name, const YAML::Node&);

	virtual std::shared_ptr<agi::AGIObject> makeNode(agi::AGIRoom*) = 0;

	friend std::ostream& operator<<(std::ostream& os, const ObjInfo& obj);

	int getRoom() const;

	void setRoom(int);

	bool hasView() const;

	std::string getView() const;
protected:
	void applyModel(std::shared_ptr<agi::AGIObject>, agi::AGIRoom*);

	std::string _name;

	int _x, _y;

	// the room the object is located (-1 = nowhere)
	int _room;

	std::string _sprite;

	std::string _anim;

	std::vector<int> _noWalkColors;

};

class DefaultObjInfo : public ObjInfo {
public:
	using ObjInfo::ObjInfo;

	std::shared_ptr<agi::AGIObject> makeNode(agi::AGIRoom*) override ;

};

class CharacterInfo : public ObjInfo {
public:
	explicit CharacterInfo(const std::string& name, const YAML::Node&);
protected:
	int _speed;
	int _direction;
	std::unordered_map<std::string, int> _inventory;
};

class PlayerInfo : public CharacterInfo {
public:
	explicit PlayerInfo(const std::string& name, const YAML::Node&);
	std::shared_ptr<agi::AGIObject> makeNode(agi::AGIRoom*) override ;

};

class NPCInfo : public CharacterInfo {
public:
	explicit NPCInfo(const std::string& name, const YAML::Node&);
	std::shared_ptr<agi::AGIObject> makeNode(agi::AGIRoom*) override ;

private:
	std::string _strategy;
};

inline int ObjInfo::getRoom() const {
	return _room;
}

inline void ObjInfo::setRoom(int roomId) {
	_room = roomId;
}

inline bool ObjInfo::hasView() const {
	return !_sprite.empty();
}

inline std::string ObjInfo::getView() const {
	return _sprite;
}
// Define global operator<<
