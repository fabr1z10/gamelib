#include "factory.h"
#include <yaml-cpp/yaml.h>
#include <iostream>
#include <gamelib/agi/agiroom.h>
#include <filesystem>

std::shared_ptr<Room> RoomFactory::createRoom() {
	std::filesystem::path p("assets");
	int roomNumber = _state.at("room");
	agi::RoomConfig config;
	config.priority = "AGI";
	config.height = 168;
	config.bgImage = p / std::to_string(roomNumber) / "view.png";
	config.controlImage = p / std::to_string(roomNumber) / "control.png";
	config.spriteSheet ="assets/kq1.yaml";
	config.fontFile ="assets/fonts.yaml";
	config.priorityImage = p / std::to_string(roomNumber) / "priority.png";
	config.wordsFile = "assets/words.yaml";

	auto room = std::make_shared<agi::AGIRoom>(config);
	for (const auto& [id, objInfo] : _objInfos) {
		if (objInfo->getRoom() == roomNumber) {
			auto node = objInfo->makeNode();
			std::cout << "adding object " << id << " to room " << roomNumber << "\n";
			if (objInfo->hasView()) {
				auto model = room->getModel(objInfo->getView());
				node->setModel(model);
			}
			room->addObject(node);
		}
	}

	return room;
}

void RoomFactory::init(std::shared_ptr<Room>) {

}
RoomFactory::RoomFactory() {
	YAML::Node config = YAML::LoadFile("../assets/objs.yaml");
	for (const auto& item: config["objects"]) {
		auto id = item.first.as<std::string>();
		auto type = item.second["type"].as<std::string>("default");
		std::shared_ptr<ObjInfo> objInfo;
		if (type == "default") {
			objInfo = std::make_shared<DefaultObjInfo>(id, item.second);
			_objInfos[id] = objInfo;
		}


	}

	for (const auto& item : config["state"]) {
		_state[item.first.as<std::string>()] = item.second.as<int>();
	}
	for (const auto& [a,b] : _objInfos) {
		std::cout << *b;
	}

}
