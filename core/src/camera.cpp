#include "gamelib/camera.h"
#include "gamelib/game.h"
#include <glm/gtc/matrix_transform.hpp>


Camera::Camera(glm::ivec4 viewport) {

	if (viewport == glm::ivec4(0)) {
		auto deviceSize = Game::instance().getConfig()->getDeviceSize();
		_viewport = glm::ivec4(0, 0, deviceSize.x, deviceSize.y);
	} else {
		_viewport = viewport;
	}

	setPosition(glm::vec3(0, 0, 0), glm::vec3(0, 0, -1), glm::vec3(0, 1, 0));
}

void Camera::setPosition(glm::vec3 eye, glm::vec3 direction, glm::vec3 up) {
	_eye = clampPosition(eye);
	_fwd = glm::normalize(direction);
	_up = glm::normalize(up);
	updateViewMatrix();
}

void Camera::updateViewMatrix() {
	_viewMatrix = glm::lookAt(_eye, _eye + _fwd, _up);
}

void Camera::move(glm::vec3 delta) {
	setPosition(_eye + delta, _fwd, _up);
}

void Camera::setBounds(glm::vec3 min, glm::vec3 max) {
	_xBounds = glm::vec2( min.x, max.x);
	_yBounds = glm::vec2( min.y, max.y );
	_zBounds = glm::vec2(min.z, max.z );
	setPosition(_eye, _fwd, _up);

}

glm::vec3 Camera::clampPosition(const glm::vec3 &pos) {
	glm::vec3 result = pos;

	if (_xBounds) result.x = std::clamp(result.x, _xBounds->x, _xBounds->y);
	if (_yBounds) result.y = std::clamp(result.y, _yBounds->x, _yBounds->y);
	if (_zBounds) result.z = std::clamp(result.z, _zBounds->x, _zBounds->y);

	return result;
}


OrthoCamera::OrthoCamera(float width, float height, float near, float far, glm::vec4 viewport) :
	Camera(viewport), _orthoSize(width, height) {
	float hw = _orthoSize.x / 2.0f;
	float hh = _orthoSize.y / 2.0f;
	_projectionMatrix = glm::ortho(-hw, hw, -hh, hh, near, far);
}

glm::vec2 OrthoCamera::getWorldCoordinates(glm::vec2 deviceCoordinates) const {
	float xw = -_viewMatrix[3][0] - (_orthoSize.x * 0.5f) + (deviceCoordinates.x - _viewport.x) * (_orthoSize.x / _viewport[2]);
	float yw = -_viewMatrix[3][1] - (_orthoSize.y * 0.5f) + (deviceCoordinates.y - _viewport.y) * (_orthoSize.y / _viewport[3]);
	return glm::vec2(xw, yw);
}



PerspectiveCamera::PerspectiveCamera(glm::vec4 viewport, float fov, float near, float far) :
	Camera(viewport), _fov(fov), _near(near), _far(far) {
	_projectionMatrix = glm::perspective (glm::radians(_fov), (float)_viewport[2]/_viewport[3], _near, _far);
}

glm::mat4 Camera::getViewProjectionMatrix() const {
	return _projectionMatrix * _viewMatrix;
}