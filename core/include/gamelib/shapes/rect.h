#pragma once

#include "../shape.h"
#include <vector>



namespace shapes {

	class Rect : public Shape {
	public:
		Rect(float width, float height, glm::vec2 anchor = glm::vec2(0,0));

		RaycastResult raycastAxis(glm::vec2 origin, float length, Axis axis) const override;

		std::shared_ptr<IModel> makeModel(IBatch*) override;

		bool isInside(glm::vec2) const override;
	private:
		glm::vec2 _anchor;
		glm::vec2 _size;
	};

}