#pragma once

#include "gamelib/node.h"
#include "gamelib/agi/priority.h"
#include <unordered_map>
#include <string>
#include <functional>
#include "gamelib/keylistener.h"

namespace agi {

	class AGIRoom;

	class AGIObject : public Node {
	public:
		using Callback = std::function<int(AGIObject*, int, int)>;

		AGIObject(const std::string& id, int x, int y);

		void adjustPriority();

		void setPriorityCalculator(std::shared_ptr<PriorityCalculator> pc) ;

		void setCallback(int, Callback);

		void reposition(int x, int y);

		std::string getId() const;
	protected:
		bool checkPixel(int x, int y);
		std::string _id;
		std::shared_ptr<PriorityCalculator> _priorityCalculator;
		AGIRoom* _room;
		std::unordered_map<int, Callback > _callbacks;

	};

	inline std::string AGIObject::getId() const {
		return _id;
	}

	class AGICharacter : public AGIObject {
	public:
		AGICharacter(const std::string& id, int x, int y, float speed);

		virtual void animate();

		virtual void move(int delta);

		void suspendMovement(bool);

		int has(const std::string& item) const;

		void addToInventory(const std::string item, int qty);

		void setModel(std::shared_ptr<IModel> model) override;

	protected:
		bool _suspendMovement = false;

		// bit 0 of direction tells whether moving (1) or not (0)
		// bit 1 is movement in x direction 1 = right, 0 = left
		// bit 2 is movement is y direction 1 = up, 0 = left

		uint8_t _direction;
		float _speed;
		std::unordered_map<int, std::string> _animationMap;
		std::unordered_map<std::string, int> _inventory;

	};

	class AGIPlayableCharacter : public AGICharacter, public KeyListener {
	public:
		using AGICharacter::AGICharacter;

		void customUpdate(double) override;

		int keyCallback(GLFWwindow*, int key, int scancode, int action, int mods) override;

	};

	inline void AGICharacter::suspendMovement(bool value) {
		_suspendMovement = value;
	}

	class AGINPC : public AGICharacter {
	public:
	};
}