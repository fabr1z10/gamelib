#pragma once

#include "room.h"

#include <memory>

class IRoomFactory {
public:
	virtual ~IRoomFactory() = default;
	virtual std::shared_ptr<Room> createRoom() = 0;
};