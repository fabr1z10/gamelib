#pragma once

#include "gamelib/room.h"
#include "gamelib/keylistener.h"
#include <string>
#include "gamelib/agi/priority.h"

namespace agi {

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
		AGIContext(const std::string& contextFile);
		// object 0 is ego!
		std::unordered_map<std::string, ObjectInfo> objects;
	};

	class AGIRoom : public Room, public KeyListener {
	public:
		AGIRoom(const std::string& id, std::shared_ptr<AGIContext>);//int id, int roomHeight, const std::string& bg, const std::string& view, PriorityMode mode);

		int keyCallback(GLFWwindow*, int key, int scancode, int action, int mods) override;

		void initialize() override;

		int getRoomHeight() const;
	private:
		std::string _roomId;
		int _roomHeight;
		int _gameWidth;
		int _gameHeight;
		std::shared_ptr<AGIContext> _agi;
		std::shared_ptr<PriorityCalculator> _priorityCalculator;
	};

	inline int AGIRoom::getRoomHeight() const {
		return _roomHeight;
	}
}
