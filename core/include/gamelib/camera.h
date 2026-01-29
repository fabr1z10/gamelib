#pragma once

#include <glm/glm.hpp>
#include <array>
#include <optional>

class Camera {
public:
	Camera(glm::ivec4 viewport = glm::ivec4(0));

	virtual ~Camera() = default;

	void setPosition(glm::vec3 eye, glm::vec3 direction, glm::vec3 up);

	void move(glm::vec3);

	void setBounds(glm::vec3 min, glm::vec3 max);

	const glm::mat4& getViewMatrix() const;

	const glm::mat4& getProjectionMatrix() const;

	glm::ivec4 getViewport() const;

	glm::vec3 getPosition() const;

	glm::mat4 getViewProjectionMatrix() const;
protected:

	glm::vec3 _fwd;
	glm::vec3 _up;
	glm::vec3 _eye;
	std::optional<glm::vec2> _xBounds;
	std::optional<glm::vec2> _yBounds;
	std::optional<glm::vec2> _zBounds;


	glm::mat4 _projectionMatrix;
	glm::mat4 _viewMatrix;
	glm::ivec4 _viewport;
private:
	glm::vec3 clampPosition(const glm::vec3& pos);
	void updateViewMatrix();
};

inline glm::ivec4 Camera::getViewport() const {
	return _viewport;
}

inline glm::vec3 Camera::getPosition() const {
	return _eye;
}

inline const glm::mat4 & Camera::getViewMatrix() const {
	return _viewMatrix;
}

inline const glm::mat4 & Camera::getProjectionMatrix() const {
	return _projectionMatrix;
}


class OrthoCamera : public Camera {
public:
	OrthoCamera(float width, float height, float near, float far, glm::vec4 viewport);

	glm::vec2 getSize() const;

	/* Transform device coordinates into world coordinates
	 */
	[[nodiscard]] glm::vec2 getWorldCoordinates(glm::vec2) const;
private:
	glm::vec2 _orthoSize;
};

inline glm::vec2 OrthoCamera::getSize() const {
	return _orthoSize;
}


class PerspectiveCamera : public Camera {
public:

	explicit PerspectiveCamera(glm::vec4 viewport, float fov, float near, float far);

private:
	float _fov;
	float _near;
	float _far;
	float _aspectRatio;
};


