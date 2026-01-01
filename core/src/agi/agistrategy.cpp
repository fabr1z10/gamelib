#include "gamelib/agi/agistrategy.h"
#include "gamelib/random.h"


using namespace agi;

uint8_t Wander::getNextDirection(uint8_t currentDirection, bool ok) {
	if (ok) return currentDirection;
	return 4 + Random::instance().nextInt(0, 12);
}