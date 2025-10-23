#include "gamelib/room.h"
#include <GL/glew.h>

Room::Room() : _clearColor(0.0f, 0.0f, 0.0f) {

}


void Room::setClearColor(glm::ivec3 color) {
	_clearColor = glm::vec3(color) / 255.0f;
}

void Room::draw() {
	glClearColor(_clearColor.r, _clearColor.g, _clearColor.b, 1.f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

}