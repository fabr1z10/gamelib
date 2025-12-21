#include "gamelib/agi/agiroom.h"
#include "gamelib/camera.h"
#include "gamelib/game.h"
#include "gamelib/model.h"
#include "gamelib/node.h"
#include "gamelib/agi/agibatch.h"
#include "gamelib/agi/agiobject.h"
//#include <yaml-cpp/yaml.h>
#include "gamelib/yaml_extension.h"
#include <yaml-cpp/node/convert.h>

using namespace agi;

AGIContext::AGIContext(const std::string& contextFile) {
	try {
		YAML::Node contextData = YAML::LoadFile(contextFile);
		for (const auto& item : contextData["objects"]) {
			objects[item.first.as<std::string>()] = item.second.as<agi::ObjectInfo>();
		}
	} catch (const YAML::BadFile &e) {
		throw std::runtime_error(std::string("Could not open AGI context file: ") + e.what());
	} catch (const YAML::Exception &e) {
		throw std::runtime_error(std::string("YAML parsing error in AGI context file: ") + e.what());
	} catch (const std::exception &e) {
		throw std::runtime_error(std::string("Error loading AGI context file: ") + e.what());
	}
	//p
	//objects.push_back(ObjectInfo());
}

AGIRoom::AGIRoom(const std::string& roomId, std::shared_ptr<AGIContext> context) : Room(), _agi(context) {//int id, int roomHeight,
	auto slashIndex = roomId.find_last_of('/');
	_roomId = roomId.substr(slashIndex + 1);
	std::string fileName = roomId.substr(0, slashIndex) + ".yaml";
	try {
		YAML::Node roomData = YAML::LoadFile(fileName);
		roomData = roomData[_roomId];
		if (!roomData) {
			throw std::runtime_error("Cannot find room: " + _roomId + " in file: " + fileName);
		}
		_roomHeight = roomData["height"].as<int>();
		auto priority = roomData["priority"].as<std::string>();
		if (priority == "AGI") {
			_priorityCalculator = std::make_shared<AGIPriorityCalculator>();
		} else if (priority == "BASIC") {
			_priorityCalculator = std::make_shared<BasicPriorityCalculator>(_roomHeight);
		} else {
			throw std::runtime_error("Unknown priority mode: " + priority);
		}
		auto picture = roomData["bg"]["picture"].as<std::string>();
		auto prio = roomData["bg"]["priority"].as<std::string>();
		auto ctrl = roomData["bg"]["control"].as<std::string>();
		auto spriteSheet = roomData["spritesheet"].as<std::string>();
		auto fonts = roomData["fonts"].as<std::string>();

		_controlImage = std::make_shared<Tex>();
		_controlImage->keepCPUCopy(true);
		_controlImage->load(ctrl);

		// create game camera
		_gameWidth = 160;
		_gameHeight = 168;
		glm::vec2 stretch(2, 1);
		auto gameCam = std::make_shared<OrthoCamera>(_gameWidth, _gameHeight, 0.f, 1.f,
		glm::vec4(0, 24, _gameWidth * stretch[0], _gameHeight * stretch[1]));
		gameCam->setBounds(glm::vec3(_gameWidth * 0.5f, _gameHeight * 0.5f, -100.f),
					  glm::vec3(_gameWidth * 0.5f, _gameHeight * 0.5f, 100.f));
		addCamera("game", gameCam);

		auto textCam = std::make_shared<OrthoCamera>(320, 200, 0.f, 1.f, glm::vec4(0, 0, 320, 200));
		textCam->setBounds(glm::vec3(160, 100, -100), glm::vec3(160, 100, 100));
		addCamera("text", textCam);

		// create shader for sprites
		auto shader = Game::instance().getShader("sprite_pal");

		// create shader for background
		auto agiShader = Game::instance().getShader("agi");

		auto bgBatch = std::dynamic_pointer_cast<AGIBatch<VertexTexturePalette, QuadPrimitive>>(agiShader->createBatch(gameCam.get(), 1024));
		auto spriteBatch = shader->createBatch(gameCam.get(), 1024);
		auto textBatch = shader->createBatch(textCam.get(), 1024);

		bgBatch->addPriority(Tex::getTexture(prio));
		addBatch("bg", bgBatch);
		addBatch("spr", spriteBatch);
		addBatch("txt", textBatch);
		bgBatch->addSpriteSheet(SpriteSheet::getSpriteSheet(picture));
		spriteBatch->addSpriteSheet(SpriteSheet::getSpriteSheet(spriteSheet));
		textBatch->addSpriteSheet(SpriteSheet::getSpriteSheet(fonts));

	} catch (const YAML::BadFile &e) {
		throw std::runtime_error(std::string("Could not open room file: ") + e.what());
	} catch (const YAML::Exception &e) {
		throw std::runtime_error(std::string("YAML parsing error: ") + e.what());
	} catch (const std::exception &e) {
		throw std::runtime_error(std::string("Error loading room: ") + e.what());
	}
	return;

//	_roomHeight = 168; // TODO: read from config
//	std::string fullName = Game::instance().getHomeDir() + "/" + filename;
//
//	auto image = YAML::read<std::string>(config, "image");
//	//const std::string &bg,
//	//const st
//	//const std::string &view,
//	//PriorityMode priorityMode) : Room(), _id(id), _roomHeight(roomHeight) {
//
//
//
//	switch (priorityMode) {
//		case PRIORITY_BASIC:
//			_priorityCalculator = std::make_shared<BasicPriorityCalculator>(_roomHeight);
//			break;
//		case PRIORITY_AGI:
//			_priorityCalculator = std::make_shared<AGIPriorityCalculator>();
//			break;
//		default:
//			_priorityCalculator = std::make_shared<BasicPriorityCalculator>(_roomHeight);
//			break;
//	}
//







}

int AGIRoom::keyCallback(GLFWwindow *, int key, int scancode, int action, int mods) {
	return 0;
}

int AGIRoom::test(int x, int y) {
	int color = _controlImage->getIndex(x, _controlImage->getSize()[1]-y);
	return color;
}

void AGIRoom::addObject(std::shared_ptr<agi::AGIObject> node) {
	node->setPriorityCalculator(_priorityCalculator);
	this->getRootNode()->add(node);

}

void AGIRoom::initialize() {
	Room::initialize();
	QuadInfo info(_gameWidth, _gameHeight);
	auto model = std::make_shared<QuadModelPal>(getBatch("bg"), info);
	auto node = std::make_shared<Node>();
	node->setModel(model);
	this->getRootNode()->add(node);

	// adding objects
/*	auto spriteBatch = getBatch("spr");
	for (const auto& [key, objInfo] : _agi->objects) {
		if (objInfo.room == _roomId) {
			auto model = spriteBatch->getModel(objInfo.model);
			auto node = std::make_shared<agi::AGIObject>(objInfo.x, objInfo.y, 1);
			node->setPriorityCalculator(_priorityCalculator);
			node->setModel(model);
			this->getRootNode()->add(node);
		}
	}*/
}

