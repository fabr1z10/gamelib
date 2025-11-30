#pragma once

#include <string>
#include <glm/glm.hpp>
#include "gamelib/component.h"


class Camera;

class Follow : public Component {
public:
	Follow(const std::string& camId, glm::vec3 relativePos = glm::vec3(0.f, 0.f, 5.0f));


	void update(double) override;

	void start() override;
protected:
	glm::vec3 _relativePos;
	glm::vec3 _up;
	Camera* _cam;
	glm::vec3 _previous;
	std::string _camId;
};