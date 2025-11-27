#pragma once

#include <glm/glm.hpp>

struct Bounds {

	Bounds() : xm(0.f), xM(0.f), ym(0.f), yM(0.f) {}

	float xm;
	float xM;
	float ym;
	float yM;

	bool testAABB(const Bounds& other);

	Bounds transform(const glm::mat4& t);

	Bounds translate(const glm::vec2&);
};