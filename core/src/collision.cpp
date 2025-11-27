#include "gamelib/collision.h"

const float EPSILON = 1e-6f;

float cross2D(glm::vec2 u, glm::vec2 v) {
	return (u.x * v.y - u.y * v.x);
}

// returns the angle (in radians) between two vectors
float angle(glm::vec3 v1, glm::vec3 v2) {
	return std::acos(glm::dot(v1, v2));
}

float angle(glm::vec2 v1, glm::vec2 v2) {
	return std::acos(glm::dot(v1, v2));
}
