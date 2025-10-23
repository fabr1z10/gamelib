#pragma once

#include <glm/glm.hpp>

class Room {
public:
	Room();

	void update(double) {
		// default: do nothing
	}

	void draw();

	bool isComplete() const;

	/*
	 * Set clear color passing r,g,b values as integers in range 0-255
	 */
	void setClearColor(glm::ivec3);

private:
	glm::vec3 _clearColor;
	bool _complete = false;
};

inline bool Room::isComplete() const {
	return _complete;
}

