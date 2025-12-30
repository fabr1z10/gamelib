#pragma once

#include "glm/glm.hpp"
#include <memory>
#include "gamelib/collision.h"
#include "gamelib/bounds.h"

class IModel;
class IBatch;

enum ShapeType {
	POINT, LINE, POLYLINE, POLYGON, RECT, AABB
};

enum ModelType {
	WIREFRAME, SOLID
};

class Shape {
public:
	virtual ~Shape() {};

	virtual RaycastResult raycastAxis(
			glm::vec2 origin,
			float length,
			Axis axis) const = 0;

	virtual std::shared_ptr<IModel> makeModel(IBatch*, glm::vec4 color, ModelType) = 0;

	virtual bool isInside(glm::vec2) const = 0;

	Bounds getBounds() const;

	ShapeType getType() const;
protected:
	ShapeType _type;
	Bounds _bounds;
};

inline Bounds Shape::getBounds() const {
	return _bounds;
}

inline ShapeType Shape::getType() const {
	return _type;
}