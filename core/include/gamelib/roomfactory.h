#pragma once

#include "room.h"

#include <memory>

class IRoomFactory {
public:
	virtual ~IRoomFactory() = default;
	virtual void createRoom() = 0;
	virtual void init(std::shared_ptr<Room>) = 0;
	std::string getString(const std::string& id) const {
		return _strings.at(id);
	}
protected:
	std::unordered_map<std::string, std::string> _strings;
	std::unordered_map<std::string, int> _variables;
};