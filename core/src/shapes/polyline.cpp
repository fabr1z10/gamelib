#include "gamelib/shapes/polyline.h"
#include "gamelib/model.h"

using namespace shapes;

PolyLine::PolyLine(const std::vector<glm::vec2> &points, bool closed) : Shape(), _points(points), _closed(closed) {
	_type = ShapeType::POLYLINE;
	_bounds.xm = points[0].x;
	_bounds.xM = points[0].x;
	_bounds.ym = points[0].y;
	_bounds.yM = points[0].y;
	for (const auto& p : points) {
		_bounds.xm = std::min(_bounds.xm, p.x);
		_bounds.xM = std::max(_bounds.xM, p.x);
		_bounds.ym = std::min(_bounds.ym, p.y);
		_bounds.yM = std::max(_bounds.yM, p.y);
	}
}


RaycastResult PolyLine::raycastAxis(glm::vec2 origin, float length, Axis axis) const {
	RaycastResult out;
	return out;
}

bool PolyLine::isInside(glm::vec2 P) const {
	return true;
}


// mode is ignored for line and polyline
std::shared_ptr<IModel> PolyLine::makeModel(IBatch* batch) {
	return std::make_shared<LineModel>(batch, _points, glm::vec4(1.f), false);
}

