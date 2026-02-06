#include "gamelib/agi/agiroom.h"
#include "gamelib/camera.h"
#include "gamelib/game.h"
#include "gamelib/model.h"
#include "gamelib/node.h"
#include "gamelib/agi/agibatch.h"
#include "gamelib/agi/agiobject.h"
#include "gamelib/yaml_extension.h"
#include <yaml-cpp/node/convert.h>
#include "gamelib/text.h"
#include "gamelib/shapes/rect.h"

using namespace agi;

AGIContext &AGIContext::instance() {
	static AGIContext instance;
	return instance;
}

std::string AGIContext::getString(const std::string &id) {
	return _strings.at(id);
}

AGIContext::AGIContext() {
	try {
		YAML::Node contextData = YAML::LoadFile("../assets/strings.yaml");

		for (const auto& item : contextData) {
			_strings[item.first.as<std::string>()] = item.second.as<std::string>();
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



AGIRoom::AGIRoom(const RoomConfig& cfg) : Room(), _agi(AGIContext::instance()),
	_pauseKey(GLFW_KEY_F1) {
	_cursor = "_";
	_command = "";
	_prompt = ">";
	auto path = Game::instance().getHomeDir() / "assets" / "script.gs";
	_languageParser = std::make_unique<agi::LanguageParser>(path);
	_roomHeight = cfg.height;

	if (cfg.priority == "AGI") {
		_priorityCalculator = std::make_shared<AGIPriorityCalculator>();
	} else if (cfg.priority == "BASIC") {
		_priorityCalculator = std::make_shared<BasicPriorityCalculator>(_roomHeight);
	} else {
		throw std::runtime_error("Unknown priority mode: " + cfg.priority);
	}

	std::cout << " -- loading control image: " << cfg.controlImage << "\n";
	_controlImage = std::make_shared<Tex>();
	_controlImage->keepCPUCopy(true);
	_controlImage->load(Game::instance().getHomeDir() / cfg.controlImage);
	_gridGraph = std::make_unique<GridGraph>(*_controlImage);

	// create game camera
	_gameWidth = 160;
	_gameHeight = _roomHeight;
	glm::vec2 stretch(2, 1);
	auto gameCam = std::make_shared<OrthoCamera>(_gameWidth, _gameHeight, 0.f, 1.f,
		glm::vec4(0, 24, _gameWidth * stretch[0], _gameHeight * stretch[1]));
	gameCam->setBounds(glm::vec3(_gameWidth * 0.5f, _gameHeight * 0.5f, -100.f),
		glm::vec3(_gameWidth * 0.5f, _gameHeight * 0.5f, 100.f));
	addCamera("game", gameCam);

	if (Game::instance().getConfig()->mouseOn()) {
		_mouse = std::make_unique<AGIMouse>(this, gameCam.get());
		Game::instance().registerToMouseEvent(_mouse.get());
	}

	auto textCam = std::make_shared<OrthoCamera>(320, 200, 0.f, 1.f, glm::vec4(0, 0, 320, 200));
	textCam->setBounds(glm::vec3(160, 100, -100), glm::vec3(160, 100, 100));
	addCamera("text", textCam);

	// create shader for sprites
	auto shader = Game::instance().getShader("sprite_pal");
	auto lineShader = Game::instance().getShader("line_color");
	auto triShader = Game::instance().getShader("triangle_color");
	auto agiShader = Game::instance().getShader("agi");

	auto bgBatch = std::dynamic_pointer_cast<AGIBatch<VertexTexturePalette, QuadPrimitive>>(agiShader->createBatch(gameCam.get(), 1024));
	auto spriteBatch = shader->createBatch(gameCam.get(), 1024);
	auto textBatch = shader->createBatch(textCam.get(), 1024);
	auto lineBatch = lineShader->createBatch(textCam.get(), 4096);
	auto triBatch = triShader->createBatch(textCam.get(), 4096);

	bgBatch->addPriority(Tex::getTexture(Game::instance().getHomeDir() / cfg.priorityImage));
	addBatch("bg", bgBatch);
	addBatch("spr", spriteBatch);
	addBatch("txt", textBatch);
	addBatch("line", lineBatch);
	addBatch("tri", triBatch);
	bgBatch->addSpriteSheet(SpriteSheet::getSpriteSheet(Game::instance().getHomeDir() / cfg.bgImage));
	spriteBatch->addSpriteSheet(SpriteSheet::getSpriteSheet(Game::instance().getHomeDir() / cfg.spriteSheet));
	textBatch->addSpriteSheet(SpriteSheet::getSpriteSheet(Game::instance().getHomeDir() / cfg.fontFile));
	_parser = std::make_shared<AGITokenParser>(this, Game::instance().getHomeDir() / cfg.wordsFile);
}

void AGIRoom::executeMacro(agi::Macro &macro) {
	auto inst = _languageParser->expandMacro(macro);
	// ok, now I have a list of instructions to execute. For now, I just print them, but eventually I will need to implement an instruction executor that can handle all the different types of instructions (e.g. show message, move object, etc.)
	for (const auto &i: inst) {
		std::cout << i;
	}
}

int AGIRoom::keyCallback(GLFWwindow *, int key, int scancode, int action, int mods) {
	if (!_paused) {
		bool shift = (mods & GLFW_MOD_SHIFT) || (mods & GLFW_MOD_CAPS_LOCK);
		if (action == GLFW_PRESS) {
			if (key == GLFW_KEY_F10) {
				this->close();
				return 0;
			}
			if (key == _pauseKey) {
				pause(!_paused);
				return 0;
			}
			if (key == GLFW_KEY_ENTER) {
				std::string c(_command);
				_command.clear();
				updateCommandText();
				_parser->parse(c);
				// TODO parseCommand(c);
				return 0;
			}
			if (key == GLFW_KEY_BACKSPACE) {
				if (_command.size() > 0) {
					_command.pop_back();
					updateCommandText();

				} else {
					return 0;
				}
			} else {
				if (key >= GLFW_KEY_A && key <= GLFW_KEY_Z) {
					if (!shift) {
						key += 0x20;
					}
					_command += char(key);
					updateCommandText();

				} else if (key >= GLFW_KEY_0 && key <= GLFW_KEY_9 || key == GLFW_KEY_SPACE) {
					_command += char(key);
					updateCommandText();

				}

			}
		}
	} else {
		if (action == GLFW_PRESS) {
			if (key == _pauseKey || key == GLFW_KEY_ENTER) {
				if (_msgNode != nullptr) {
					_msgNode->remove();
					_msgNode = nullptr;
				}
				pause(!_paused);
				return 0;
			}
		}
	}
}

int AGIRoom::test(int x, int y) {
	auto size = _controlImage->getSize();
	if (x < 0 || x >= size.x ||
		y < 0 || y >= size.y) {
		return 0;
	}
	int color = _controlImage->getIndex(x, _controlImage->getSize()[1]-1-y);
	return color;
}

void AGIRoom::addObject(std::shared_ptr<agi::AGIObject> node) {
	node->setPriorityCalculator(_priorityCalculator);
	this->getRootNode()->add(node);
	_objectMap[node->getId()] = node.get();
}

void AGIRoom::rmObject(const std::string & id) {
	_objectMap.at(id)->remove();
	_objectMap.erase(id);
}

agi::AGIObject *AGIRoom::getObject(const std::string &id) {
	return _objectMap.at(id);
}

std::shared_ptr<IModel> AGIRoom::getModel(const std::string &id) {
	return getBatch("spr")->getModel(id);
}

void AGIRoom::updateCommandText() {
	auto cmd = _prompt + _command + _cursor;
	_commandText->updateText(cmd);
}

void AGIRoom::initialize() {
	Room::initialize();

	QuadInfo info(_gameWidth, _gameHeight);
	auto model = std::make_shared<QuadModelPal>(getBatch("bg"), info);
	auto node = std::make_shared<Node>();
	node->setModel(model);
	this->getRootNode()->add(node);


	// add command text
	auto text = std::make_shared<Text>(getBatch("txt"), "sierra", "", 0, HAlign::LEFT, 40, Anchor::TOP_LEFT);
	text->setPosition(glm::vec3(0.f, 24.f, 0.f));
	this->getRootNode()->add(text);
	_commandText = text.get();
	updateCommandText();
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

void AGIRoom::addSaid(const std::vector<std::string> &words, std::shared_ptr<Macro> m) {
	_parser->addSaid(words, m);
}

void AGIRoom::addRect(float x, float y, float z, int width, int height, glm::vec4 color, ModelType mtype, Node* parent) {
	auto shape = std::make_shared<shapes::Rect>(width, height);
	auto model = shape->makeModel(mtype == ModelType::WIREFRAME ? getBatch("line") : getBatch("tri"), color, mtype);
	auto node = std::make_shared<Node>();
	node->setPosition(glm::vec3(x, y, z));
	node->setModel(model);
	parent->add(node);
}

std::shared_ptr<Node> AGIRoom::createMessage(const std::string& msg) {
	auto messageNode = std::make_shared<Node>();

	auto text = std::make_shared<Text>(getBatch("txt"), "sierra", msg, 1 /* TODO MAKE IT CONFIGURABLE */,
		HAlign::LEFT, 30, Anchor::CENTER);
	auto size = text->getSize();
	text->setPosition(glm::vec3(160, 100, 0));

	messageNode->add(text);
	addRect(
			160.f - size.x * 0.5f - _msgPaddingX,
			100.f - size.y * 0.5f - _msgPaddingY,
			-0.01f,
			size.x + 2 * _msgPaddingX,
			size.y + 2 * _msgPaddingY,
			glm::vec4(1.f),
			ModelType::SOLID,
			messageNode.get());
	addRect(
			160.f - size.x * 0.5f - 8.f,
			100.f - size.y * 0.5f - 4.f,
			-0.f,
			size.x + 2 * 8, size.y + 2 * 4,
			glm::vec4(170/255.f, 0.f, 0.f, 1.f),
			ModelType::WIREFRAME,
			messageNode.get());
	addRect(
			160.f - size.x * 0.5f - 7.f,
			100.f - size.y * 0.5f - 4.f,
			-0.f,
			size.x + 2 * 7, size.y + 2 * 4,
			glm::vec4(170/255.f, 0.f, 0.f, 1.f),
			ModelType::WIREFRAME,
			messageNode.get());
	return messageNode;
}

void AGIRoom::print(const std::string & id) {
	auto msg = _agi.getString(id);
	printMessage(msg);
}

void AGIRoom::printMessage(const std::string &msg) {
	auto messageNode = createMessage(msg);
	getRootNode()->add(messageNode);
	_msgNode = messageNode.get();
	pause(true);
}

void AGIRoom::showObject(const std::string &view, const std::string &id) {
	auto msg = _agi.getString(id);

	auto messageNode = createMessage(msg);
	auto img = std::make_shared<Node>();
	img->setModel(getBatch("spr")->getModel(view));
	img->setPosition(glm::vec3(80, 0, 0));
	messageNode->add(img);
	getRootNode()->add(messageNode);
	_msgNode = messageNode.get();
	pause(true);
}