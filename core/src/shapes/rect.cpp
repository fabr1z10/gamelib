#include "gamelib/shapes/rect.h"
#include "gamelib/model.h"

using namespace shapes;

Rect::Rect(float width, float height, glm::vec2 anchor) : Shape(),
_size(width, height), _anchor(anchor) {
	_type = ShapeType::RECT;
	_bounds.xm = -anchor.x;
	_bounds.ym = -anchor.y;
	_bounds.xM = -anchor.x + width;
	_bounds.yM = -anchor.y + height;

}

RaycastResult Rect::raycastAxis(glm::vec2 origin, float length, Axis axis) const {
	auto localOrigin = origin += _anchor;  // move to local space
	if (axis == Axis::X) {
		if (localOrigin.y <= 0.f || localOrigin.y >= _size.y) {
			return RaycastResult{false, 0.f};
		} else {
			if (length > 0.f) {
				// ray to the right
				if (localOrigin.x <= 0.f) {
					float dist = -localOrigin.x;
					if (dist <= length) {
						return RaycastResult{true, dist};
					}
				} else if (localOrigin.x <= _size.x) {
					return RaycastResult{true, 0.f};
				}
			} else {
				// ray to the left
				if (localOrigin.x >= _size.x) {
					float dist = localOrigin.x - _size.x;
					if (dist <= -length) {
						return RaycastResult{true, dist};
					}
				} else if (localOrigin.x >= 0.f) {
					return RaycastResult{true, 0.f};
				}
			}
		}
	} else {
		// Y axis
		if (localOrigin.x <= 0.f || localOrigin.x >= _size.x) {
			return RaycastResult{false, 0.f};
		} else {
			if (length > 0.f) {
				// ray upwards
				if (localOrigin.y <= 0.f) {
					float dist = -localOrigin.y;
					if (dist <= length) {
						return RaycastResult{true, dist};
					}
				} else if (localOrigin.y <= _size.y) {
					return RaycastResult{true, 0.f};
				}
			} else {
				// ray downwards
				if (localOrigin.y >= _size.y) {
					float dist = localOrigin.y - _size.y;
					if (dist <= -length) {
						return RaycastResult{true, dist};
					}
				} else if (localOrigin.y >= 0.f) {
					return RaycastResult{true, 0.f};
				}
			}
		}
	}
	return RaycastResult{false, 0.f};
}


bool Rect::isInside(glm::vec2 P) const {
	P += _anchor; // move to local space
	return (P.x >= 0.f && P.x <= _size.x &&
			P.y >= 0.f && P.y <= _size.y);
}

std::shared_ptr<IModel> Rect::makeModel(IBatch * batch, glm::vec4 color, ModelType modelType) {
	glm::vec2 bl = -_anchor;
	glm::vec2 br = -_anchor + glm::vec2(_size.x, 0.f);
	glm::vec2 tr = -_anchor + glm::vec2(_size.x, _size.y);
	glm::vec2 tl = -_anchor + glm::vec2(0.f, _size.y);
	if (modelType == ModelType::WIREFRAME) {
		std::vector<glm::vec2> points{ bl, br, tr, tl};
		return std::make_shared<LineModel>(batch, points, color, true);
	} else {
		std::vector<glm::vec2> points {bl, br, tl, tr, tl, br};
		return std::make_shared<TriangleModel>(batch, points, color);
	}
}