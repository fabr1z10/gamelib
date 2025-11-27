#include "gamelib/shapes/line.h"
#include "../error.h"
#include "gamelib/model.h"

using namespace shapes;

Line::Line(glm::vec2 A, glm::vec2 B) : Shape(), _A(A), _B(B) {
	_type = ShapeType::LINE;
	if (A.x < B.x) {
		_x0 = A.x;
		_x1 = B.x;
	} else {
		_x0 = B.x;
		_x1 = A.x;
	}
	if (A.y < B.y) {
		_y0 = A.y;
		_y1 = B.y;
	} else {
		_y0 = B.y;
		_y1 = A.y;
	}
	_bounds.xm = std::min(A.x, B.x);
	_bounds.xM = std::max(A.x, B.x);
	_bounds.ym = std::min(A.y, B.y);
	_bounds.yM = std::max(A.y, B.y);
}


RaycastResult Line::raycastAxis(glm::vec2 origin, float length, Axis axis) const {
	RaycastResult out;

	float dir = (length >= 0.f ? 1.f : -1.f);
	float maxDist = std::abs(length);

	// Select components depending on axis
	const int rayAxis = (axis == Axis::X ? 0 : 1);
	const int othAxis = 1 - rayAxis;

	float rayOrigA   = origin[rayAxis];
	float rayOrigB   = origin[othAxis];

	float A_a = _A[rayAxis];
	float A_b = _A[othAxis];

	float B_a = _B[rayAxis];
	float B_b = _B[othAxis];

	// If segment is parallel to the ray direction, no hit
	if (A_a == B_a)
		return out;

	// Solve intersection on ray axis:
	// rayOrigA + t*dir = A_a + u*(B_a - A_a)
	float denom = (B_a - A_a);
	float t = (A_a - rayOrigA) / dir;

	if (t < 0.f || t > maxDist)
		return out;

	// Solve segment parameter
	float u = (t * dir + rayOrigA - A_a) / denom;
	if (u < 0.f || u > 1.f)
		return out;

	// Compute intersection point
	float hit_b = A_b + u * (B_b - A_b);

	// Check if segment spans ray's fixed coordinate (othAxis)
	float minB = std::min(A_b, B_b);
	float maxB = std::max(A_b, B_b);
	if (rayOrigB < minB || rayOrigB > maxB)
		return out;

	// Hit confirmed
	out.hit = true;
	out.distance = t;

//	glm::vec2 pt = origin;
//	pt[rayAxis] += t * dir;
//	out.point = pt;

	return out;
}

bool Line::isInside(glm::vec2 P) const {
	double crossProduct = cross2D(_B - _A, P - _A);
	if (std::fabs(crossProduct) > EPSILON) return false; // Not collinear

	if (P.x + EPSILON < _x0 || P.x - EPSILON > _x1 ||
		P.y + EPSILON < _y0 || P.y - EPSILON > _y1)
		return false;

	return true;
}


// mode is ignored for line and polyline
std::shared_ptr<IModel> Line::makeModel(IBatch* batch) {
	std::vector<glm::vec2> points {_A, _B};
	return std::make_shared<LineModel>(batch, points, glm::vec4(1.f), false);
}

