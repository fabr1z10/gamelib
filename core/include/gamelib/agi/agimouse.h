#pragma once

#include "gamelib/mouselistener.h"
#include "gamelib/camera.h"

namespace agi {

	class AGIRoom;

	class AGIMouse : public MouseListener {
	public:
		AGIMouse(AGIRoom* room, Camera* mainCamera);

		~AGIMouse() = default;

		// gets mouse position (0, 0) is topleft of game window
		void cursorPosCallback(GLFWwindow*, double x, double y) override;

		int mouseButtonCallback(GLFWwindow*, int button, int action, int mods) override;
	private:
		OrthoCamera* _mainCam;
		agi::AGIRoom* _room;
		glm::vec2 _mouseWorldPos;
	};
}