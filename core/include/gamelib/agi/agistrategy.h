#pragma once

#include <cstdint>

namespace agi {

	class AGIObject;

	class NPCStrategy {
	public:
		NPCStrategy() = default;

		void setObject(AGIObject* obj) { _object = obj; }

		virtual uint8_t getNextDirection(uint8_t currentDirection, bool ok) = 0;
	protected:
		AGIObject* _object;

	};

	class Wander : public NPCStrategy {
	public:
		using NPCStrategy::NPCStrategy;

		uint8_t getNextDirection(uint8_t currentDirection, bool ok) override;

	};

	// TODO follow
	class Follow : public NPCStrategy {
	public:
		using NPCStrategy::NPCStrategy;

		uint8_t getNextDirection(uint8_t currentDirection, bool ok) override;
	};
}