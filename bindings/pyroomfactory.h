#pragma once

#include <pybind11/pybind11.h>
#include "gamelib/roomfactory.h"

class PyRoomFactory : public IRoomFactory {
public:
	using IRoomFactory::IRoomFactory;

	std::shared_ptr<Room> createRoom() override {
		PYBIND11_OVERRIDE_PURE(
				std::shared_ptr<Room>, // Return type
				IRoomFactory,          // Parent class
				createRoom,            // Method name in Python
		);
	}
};