#include "gamelib/transform.h"
#include <glm/gtx/transform.hpp>

Bounds Transform::transform(const Bounds& b) {
	Bounds result;
	result.xm = scale * b.xm;
	result.xM = scale * b.xM;
	if (flipX < 0.f) {
		std::swap(result.xm, result.xM);
		result.xm = -result.xm;
		result.xM = -result.xM;
	}
	result.ym = scale * b.ym;
	result.yM = scale * b.yM;
	result.xm += position.x;
	result.xM += position.x;
	result.ym += position.y;
	result.yM += position.y;
	return result;
}

// create identity matrix
Matrix::Matrix() : _matrix(1.0f) {
}

Matrix::Matrix(glm::mat4 m) : _matrix(m) {
}

void Matrix::translate(const glm::vec3& t) {
	_matrix = glm::translate(_matrix, t);
}

void Matrix::rotate(float angle, const glm::vec3& axis) {
	_matrix = glm::rotate(_matrix, angle, axis);
}
void Matrix::scale(const glm::vec3& s) {
	_matrix = glm::scale(_matrix, s);
}

glm::vec3 Matrix::operator*(const glm::vec3& v) const {
	glm::vec4 tmp(v, 1.0f);
	tmp = _matrix * tmp;
	return glm::vec3(tmp);
}

glm::vec4 Matrix::operator*(const glm::vec4& v) const {
	return _matrix * v;
}