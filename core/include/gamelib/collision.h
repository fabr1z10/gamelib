#pragma once

#include <glm/glm.hpp>

extern const float EPSILON; // Declaration
constexpr double PI = 3.14159265358979323846;

constexpr double deg2rad(double deg) {
	return deg * PI / 180.0;
}


class Node;

enum Axis {
	X, Y
};

template <class T>
bool isEqual (T x, T y, T eps = EPSILON) {
	return std::abs(x-y) < eps;
}

template <class T>
bool isZero (T x, T eps = EPSILON) {
	return isEqual(x, 0.0f, eps);
}

float cross2D(glm::vec2 u, glm::vec2 v);
float angle(glm::vec3 v1, glm::vec3 v2) ;
float angle(glm::vec2 v1, glm::vec2 v2) ;
// returns 1 if f >= 0, -1 otherwise
inline float signf (float f) {
	return f >= 0.0f ? 1.f : -1.f;
}
struct RaycastResult {
	bool hit = false;
	float distance = 0.f;
	glm::vec2 normal = glm::vec2(0.f, 0.f);

	// Allow `if (result)` to work
	explicit operator bool() const noexcept {
		return hit;
	}
};



class CollisionResponse {
public:
	virtual ~CollisionResponse() = default;

	virtual void onStart(Node*, Node*) = 0;

	virtual void onEnd(Node*, Node*) = 0;

};