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

	void init(std::shared_ptr<Room> room) override {
		PYBIND11_OVERRIDE_PURE(
				void,                  // Return type
				IRoomFactory,          // Parent class
				init,                  // Method name in Python
				room                   // Argument(s)
		);
	}
};