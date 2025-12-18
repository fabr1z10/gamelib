#include "gamelib/agi/priority.h"


using namespace agi;

float BasicPriorityCalculator::computeZ(glm::vec3 position) {
	// in this basic calculator, we have:
	// y = 0 --> z = 0 (top)
	// y = H --> z = -1 (bottom)
	float z = - (position.y / static_cast<float>(_roomHeight));
	return z;
}

AGIPriorityCalculator::AGIPriorityCalculator() : PriorityCalculator(168), _yRanges{
		0, 48, 60, 72, 84, 96, 108, 120, 132, 144, 156, 168} {
	for (size_t i = 0; i < 11; i++) {
		_zBase[i] = -1.f + (_yRanges[i] / static_cast<float>(_roomHeight));
	}

}
/**
 *	Every Y range has a z_base which is simply - 1 + y0 / roomheight
 *	Priority 	Y Range			Z
 * 	4 			0-47			-1 (background)
 *  5 	 		48-59
 *  6 			60-71
 *  7			72-83
 *  8 			84-95
 *  9 			96-107
 *  10 			108-119
 *  11 			120-131
 *  12 			132-143
 *  13 			144-155
 *  14 			156-167
 *  15 			-
 */
float AGIPriorityCalculator::computeZ(glm::vec3 position) {
	// invert y because AGI coordinates have 0,0 at top left
	float yt = (_roomHeight - 1) - position.y;
	int bracket = 0;
	if (yt >= 48) {
		bracket = 1 + (yt - 48) / 12;
	}
	// add an adjustment to guarantee characters are drawn on top of bg
	float z = _zBase[bracket] + ((yt - _yRanges[bracket]) / 12.f) * (1.f / 12.f);
	return z;
}