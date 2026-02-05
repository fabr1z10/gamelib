#pragma once

#include "room.h"

#include <memory>

class IRoomFactory {
public:
	virtual ~IRoomFactory() = default;
	virtual void createRoom() = 0;
	virtual void init(std::shared_ptr<Room>) = 0;
};