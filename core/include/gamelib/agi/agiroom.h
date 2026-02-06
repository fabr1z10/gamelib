#pragma once

#include <string>
#include "gamelib/room.h"
#include "gamelib/keylistener.h"
#include "gamelib/agi/priority.h"
#include "gamelib/agi/agiparser.h"
#include "gamelib/tex.h"
#include "gamelib/text.h"
#include "gamelib/shape.h"
#include "gamelib/algo/gridgraph.h"
#include "gamelib/agi/agimouse.h"
#include "gamelib/agi/langparser.h"

namespace agi {

	class AGIObject;

	struct ObjectInfo {
		std::string model = "0";
		int x = 131;
		int y = 76;
		std::string room;
	};

	enum PriorityMode {
		PRIORITY_BASIC,
		PRIORITY_AGI
	};

	class AGIContext {
	public:
		static AGIContext& instance();

		std::string getString(const std::string& id);
	private:
		AGIContext();

		std::unordered_map<std::string, std::string> _strings;
	};


	struct RoomConfig {
		int height;
		std::string priority;

		std::string controlImage;
		std::string priorityImage;
		std::string bgImage;
		std::string spriteSheet;
		std::string fontFile;
		std::string wordsFile;

	};

	class AGIRoom : public Room, public KeyListener {
	public:
		AGIRoom(const RoomConfig& cfg);//int id, int roomHeight, const std::string& bg, const std::string& view, PriorityMode mode);

		int keyCallback(GLFWwindow*, int key, int scancode, int action, int mods) override;

		void initialize() override;

		int getRoomHeight() const;

		int test(int x, int y);

		std::string getId() const;

		void addObject(std::shared_ptr<agi::AGIObject>);

		void rmObject(const std::string&);

		agi::AGIObject* getObject(const std::string& id);

		std::shared_ptr<IModel> getModel(const std::string& id);

		void executeMacro(Macro& macro);

		void addSaid(const std::vector<std::string>& words, std::shared_ptr<Macro> macro);

		void print(const std::string&);

		void printMessage(const std::string& msg);

		void showObject(const std::string& view, const std::string& msg);

		void addRect(float x, float y, float z, int width, int height, glm::vec4 color, ModelType, Node* parent);

		const GridGraph& getGraph() const;

		agi::LanguageParser* getLanguageParser() {
			return _languageParser.get();
		}
	private:
		std::shared_ptr<Node> createMessage(const std::string&);

		void updateCommandText();
		std::string _roomId;
		int _roomHeight;
		int _gameWidth;
		int _gameHeight;
		AGIContext& _agi;
		std::shared_ptr<PriorityCalculator> _priorityCalculator;
		std::shared_ptr<Tex> _controlImage;
		std::string _command;
		std::string _cursor;
		std::string _prompt;
		Text* _commandText;
		int _pauseKey;

		std::shared_ptr<AGIParser> _parser;
		int _msgPaddingX = 10;
		int _msgPaddingY = 5;
		Node* _msgNode = nullptr;
		std::unordered_map<std::string, agi::AGIObject*> _objectMap;
		std::unique_ptr<GridGraph> _gridGraph;
		std::unique_ptr<agi::AGIMouse> _mouse;
		std::unique_ptr<agi::LanguageParser> _languageParser;
	};

	inline int AGIRoom::getRoomHeight() const {
		return _roomHeight;
	}

	inline std::string AGIRoom::getId() const {
		return _roomId;
	}

	inline const GridGraph &AGIRoom::getGraph() const {
		return *_gridGraph;
	}
}
