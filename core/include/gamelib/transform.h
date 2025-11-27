#pragma once

#include <glm/glm.hpp>
#include "gamelib/bounds.h"

struct Transform {
	glm::vec3 position = glm::vec3(0.0f);
	float scale	= 1.0f;
	float flipX = 1.0f;   // 1.0 = normal, -1.0 = flipped

	Bounds transform(const Bounds& b);
};

class Matrix {
public:
	Matrix();

	explicit Matrix(glm::mat4 m);

	void translate(const glm::vec3& t);

	void rotate(float angle, const glm::vec3& axis);

	void scale(const glm::vec3& s);

	glm::mat4 getMatrix() const;

	glm::vec3 operator*(const glm::vec3& v) const;

	glm::vec4 operator*(const glm::vec4& v) const;

	glm::mat4 _matrix;
};

inline glm::mat4 Matrix::getMatrix() const {
	return _matrix;
}