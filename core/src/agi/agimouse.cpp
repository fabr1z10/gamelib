#include "gamelib/agi/agimouse.h"
#include <iostream>
#include "gamelib/game.h"
#include "gamelib/agi/agiroom.h"
#include "gamelib/algo/astar.h"
#include "gamelib/agi/agiobject.h"
#include "gamelib/agi/agiactions.h"

using namespace agi;

AGIMouse::AGIMouse(AGIRoom* room, Camera* mainCamera) : MouseListener(),
	_room(room), _mainCam(dynamic_cast<OrthoCamera*>(mainCamera)) {
}

void AGIMouse::cursorPosCallback(GLFWwindow *, double x, double y) {
	auto devCoords = Game::instance().getDeviceCoordinates({x, y});
	auto viewport = _mainCam->getViewport();
	bool isInViewport = devCoords.x >= viewport.x && devCoords.x <= viewport.x + viewport[2] &&
						devCoords.y >= viewport.y && devCoords.y <= viewport.y + viewport[3];
	if (isInViewport) {
		auto worldCoords = _mainCam->getWorldCoordinates(devCoords);
		_mouseWorldPos = worldCoords;;
		std::cout << "world coords: " << worldCoords.x << ", " << worldCoords.y << "\n";
	}
}

int AGIMouse::mouseButtonCallback(GLFWwindow *, int button, int action, int mods) {
	if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS) {

		auto pos = _room->getObject("ego")->getWorldPosition();
		Point start{(int) pos.x, (int) pos.y};
		Point goal{(int) _mouseWorldPos.x, (int) _mouseWorldPos.y};
		dynamic_cast<AGICharacter*>(_room->getObject("ego"))->walkTo(goal);

	}
}