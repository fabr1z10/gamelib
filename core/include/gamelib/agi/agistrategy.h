#pragma once

#include <cstdint>

namespace agi {
	class NPCStrategy {
	public:
		virtual uint8_t getNextDirection(uint8_t currentDirection, bool ok) = 0;
	};

	class Wander : public NPCStrategy {
	public:
		uint8_t getNextDirection(uint8_t currentDirection, bool ok) override;

	};
}