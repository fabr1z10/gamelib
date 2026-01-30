#include "gamelib/agi/agistrategy.h"
#include "gamelib/random.h"
#include "gamelib/agi/agiobject.h"
#include "gamelib/agi/agiroom.h"

using namespace agi;

uint8_t Wander::getNextDirection(uint8_t currentDirection, bool ok) {
	if (ok) return currentDirection;

	auto pos = _object->getWorldPosition();
	auto ix = (int)std::floor(pos.x);
	auto iy = (int)std::floor(pos.y);
	std::vector<CharacterDirection> dirs { RIGHT, LEFT, UP, DOWN, UP_RIGHT, UP_LEFT, DOWN_RIGHT, DOWN_LEFT };

	bool rightAllowed = _object->checkPixel(ix+1, iy);
	bool leftAllowed = _object->checkPixel(ix-1, iy);
	bool upAllowed = _object->checkPixel(ix, iy+1);
	bool downAllowed = _object->checkPixel(ix, iy-1);
	std::vector<bool> allowed{
			rightAllowed,
			leftAllowed,
			upAllowed,
			downAllowed,
			_object->checkPixel(ix+1, iy+1) && (rightAllowed || upAllowed),
			_object->checkPixel(ix-1, iy+1) && (leftAllowed || upAllowed),
			_object->checkPixel(ix+1, iy-1) && (rightAllowed || downAllowed),
			_object->checkPixel(ix-1, iy-1) && (leftAllowed || downAllowed)
	};
	std::vector<CharacterDirection> dd;
	for (size_t i = 0; i < dirs.size(); i++) {
		if (allowed[i]) dd.push_back(dirs[i]);
	}

	auto dir = dd[Random::instance().nextInt(0, dd.size())];

	return dir;
}