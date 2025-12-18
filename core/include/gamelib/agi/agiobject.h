#pragma once

#include "gamelib/node.h"
#include "gamelib/agi/priority.h"
#include <unordered_map>
#include <string>

namespace agi {

	class AGIRoom;

	class AGIObject : public Node {
	public:
		AGIObject(int x, int y, float speed);

		void adjustPriority();

		void setPriorityCalculator(std::shared_ptr<PriorityCalculator> pc) ;

		void customUpdate(double) override;

		virtual void animate();
	private:

		// bit 0 of direction tells whether moving (1) or not (0)
		// bit 1 is movement in x direction 1 = right, 0 = left
		// bit 2 is movement is y direction 1 = up, 0 = left
		uint8_t _direction;
		float _speed;
		AGIRoom* _room;
		std::shared_ptr<PriorityCalculator> _priorityCalculator;
		std::unordered_map<int, std::string> _animationMap;
	};

}