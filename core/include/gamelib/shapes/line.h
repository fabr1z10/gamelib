#pragma once

#include "../shape.h"
#include <vector>



namespace shapes {

	class Line : public Shape {
	public:
		Line(glm::vec2 A, glm::vec2 B);

		RaycastResult raycastAxis(glm::vec2 origin, float length, Axis axis) const override;

		std::shared_ptr<IModel> makeModel(IBatch*, glm::vec4, ModelType) override;

		bool isInside(glm::vec2) const override;
	private:
		glm::vec2 _A;
		glm::vec2 _B;
		// ordered x
		float _x0, _x1;
		// orderer y
		float _y0, _y1;
	};

}