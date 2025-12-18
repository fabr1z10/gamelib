#pragma once

#include "glm/glm.hpp"
#include <array>

namespace agi {

	class PriorityCalculator {
	public:
		PriorityCalculator(int roomHeight) : _roomHeight(roomHeight) {}

		virtual float computeZ(glm::vec3 position) = 0;
	protected:
		int _roomHeight;
	};

	class BasicPriorityCalculator : public PriorityCalculator {
	public:
		using PriorityCalculator::PriorityCalculator;

		float computeZ(glm::vec3 position) override;
	};

	// only works with AGI games with room height 168!!!!
	class AGIPriorityCalculator : public PriorityCalculator {
	public:
		AGIPriorityCalculator();

		float computeZ(glm::vec3 position) override;
	private:
		std::array<int, 12> _yRanges;
		std::array<float, 12> _zBase;
	};

}