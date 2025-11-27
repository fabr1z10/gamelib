#pragma once

#include "../shape.h"
#include <vector>



namespace shapes {

	class PolyLine : public Shape {
	public:
		PolyLine(const std::vector<glm::vec2>& points, bool closed);

		RaycastResult raycastAxis(glm::vec2 origin, float length, Axis axis) const override;

		std::shared_ptr<IModel> makeModel(IBatch*) override;

		bool isInside(glm::vec2) const override;
	private:
		std::vector<glm::vec2> _points;
		bool _closed;

	};

}