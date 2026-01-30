#pragma once

#include "gamelib/node.h"
#include "gamelib/agi/priority.h"
#include <unordered_map>
#include <unordered_set>
#include <string>
#include <functional>
#include "gamelib/keylistener.h"
#include "gamelib/agi/agistrategy.h"
#include "gamelib/algo/gridgraph.h"
#include <iostream>

namespace agi {

	enum CharacterDirection {
		RIGHT 		= 0x04,
		LEFT  		= 0x05,
		UP    		= 0x08,
		DOWN  		= 0x0A,
		UP_RIGHT 	= 0x0C,
		UP_LEFT  	= 0x0D,
		DOWN_RIGHT 	= 0x0E,
		DOWN_LEFT  	= 0x0F
	};

	class AGIRoom;

	class AGIObject;

	using Callback = std::function<int(AGIObject*, int, int)>;

	class HotSpot {
	public:
		HotSpot(int x0, int x1, int y0, int y1, Callback cb) : _callback(cb), _x0(x0), _x1(x1), _y0(y0), _y1(y1) {}

		int check(AGIObject* obj, int x, int y) const {
			std::cout << "HotSpot check: (" << x << ", " << y << ") in ["
					  << _x0 << "," << _x1 << "] x [" << _y0 << "," << _y1 << "]\n";
			if (x >= _x0 && x <= _x1 && y >= _y0 && y <= _y1) {
				_callback(obj, x, y);
				return 1;
			}
			return 0;
		}
	private:
		int _x0, _x1, _y0, _y1;
		Callback _callback;
	};

	class AGIObject : public Node {
	public:

		AGIObject(const std::string& id, int x, int y);

		void adjustPriority();

		void setPriorityCalculator(std::shared_ptr<PriorityCalculator> pc) ;

		void setCallback(int, agi::Callback);

		void setRectCallback(int x0, int x1, int y0, int y1, agi::Callback);

		void addBlocked(int);

		void reposition(int x, int y);

		std::string getId() const;

		bool checkPixel(int x, int y);

	protected:
		std::string _id;
		std::shared_ptr<PriorityCalculator> _priorityCalculator;
		AGIRoom* _room;
		std::unordered_map<int, agi::Callback> _callbacks;
		std::vector<HotSpot> _hotspots;
		std::unordered_set<int> _blocked;
	};

	inline std::string AGIObject::getId() const {
		return _id;
	}

	class AGICharacter : public AGIObject {
	public:
		AGICharacter(const std::string& id, int x, int y, float speed, int direction);

		virtual void animate();

		virtual void move(double);

		void suspendMovement(bool);

		int has(const std::string& item) const;

		void addToInventory(const std::string item, int qty);

		void setModel(std::shared_ptr<IModel> model) override;

		void setDirection(uint8_t dir) { _direction = dir; }

		void walkTo(Point p);
	protected:
		void setWalkSegment(int);
		struct WalkSegment {
			Point start;
			Point end;
			int hDir;
			int vDir;
			float length;
		};
		bool _suspendMovement = false;

		// bit 0 of direction tells whether moving (1) or not (0)
		// bit 1 is movement in x direction 1 = right, 0 = left
		// bit 2 is movement is y direction 1 = up, 0 = left

		uint8_t _direction;
		float _speed;
		std::unordered_map<int, std::string> _animationMap;
		std::unordered_map<std::string, int> _inventory;
		bool _moved = false;
		std::vector<Point> _path;
		std::vector<WalkSegment> _segments;
		int _segIndex = -1;
		float _currentLength = 0.0f;
		double _timeSinceLastUpdate = 0.0f;
		double _updateTime = 0.1;
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
		using AGICharacter::AGICharacter;

		void customUpdate(double) override;

		void setStrategy(std::shared_ptr<NPCStrategy> strategy);

	private:
		std::shared_ptr<NPCStrategy> _strategy;
	};
}