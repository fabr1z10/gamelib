#include "factory.h"
#include <yaml-cpp/yaml.h>
#include <iostream>
#include <gamelib/agi/agiroom.h>
#include <filesystem>
#include <gamelib/game.h>
#include <sstream>


void RoomFactory::createRoom() {
	int roomNumber = _state.at("room");
	std::filesystem::path p("assets");
	auto roomPath = p / std::to_string(roomNumber);
	agi::RoomConfig config;
	config.priority = "AGI";
	config.height = 168;
	config.bgImage = roomPath / "view.png";
	config.controlImage = roomPath / "control.png";
	config.spriteSheet ="assets/kq1.yaml";
	config.fontFile ="assets/fonts.yaml";
	config.priorityImage = roomPath / "priority.png";
	config.wordsFile = "assets/words.yaml";

	auto room = std::make_shared<agi::AGIRoom>(config);

	Game::instance().setCurrentRoom(room);
	for (const auto& [id, objInfo] : _objInfos) {
		if (objInfo->getRoom() == roomNumber) {
			auto node = objInfo->makeNode(room.get());
			std::cout << "adding object " << id << " to room " << roomNumber << "\n";
			room->addObject(node);
		}
	}

	YAML::Node roomData = YAML::LoadFile(Game::instance().getHomeDir() / roomPath / "room.yaml");
	auto* parser = room->getLanguageParser();
	for (const auto& item: roomData["scripts"]) {
		auto words = item["w"].as<std::vector<std::string>>();
		auto script = item["script"].as<std::string>();
		std::vector<std::string> lines;
		std::string line;
		std::stringstream ss(script);
		while (std::getline(ss, line, '\n')) {
			lines.push_back(line);
		}
		std::cout << "Adding script:\n";
		auto m = std::make_shared <agi::Macro>();
		for(const auto&l : lines) {
			m->addInstruction(parser->parseInstruction(l));
		}
		room->addSaid(words, m);

		std::cout << "---\n";
	}



}

void RoomFactory::init(std::shared_ptr<Room>) {

}
RoomFactory::RoomFactory() {

	// load all global stuff here!
	//_parser = std::make_unique<LanguageParser>();

	YAML::Node config = YAML::LoadFile("../assets/objs.yaml");
	for (const auto& item: config["objects"]) {
		auto id = item.first.as<std::string>();
		auto type = item.second["type"].as<std::string>("default");
		std::shared_ptr<ObjInfo> objInfo;
		if (type == "default") {
			objInfo = std::make_shared<DefaultObjInfo>(id, item.second);
			_objInfos[id] = objInfo;
		} else if (type == "player") {
			objInfo = std::make_shared<PlayerInfo>(id, item.second);
			_objInfos[id] = objInfo;
		} else if (type == "npc") {
			objInfo = std::make_shared<NPCInfo>(id, item.second);
			_objInfos[id] = objInfo;
		} else {
			throw std::runtime_error("Unknown object type: " + type);
		}


	}

	for (const auto& item : config["state"]) {
		_state[item.first.as<std::string>()] = item.second.as<int>();
	}
	for (const auto& [a,b] : _objInfos) {
		std::cout << *b;
	}

}
