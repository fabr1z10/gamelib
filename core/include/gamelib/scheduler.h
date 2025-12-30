#pragma once

#include <list>
#include "gamelib/script.h"


class Scheduler {
public:
	Scheduler() = default;

	void update(double dt);

	void play(std::shared_ptr<Script>);

private:
	std::list<std::shared_ptr<Script>> _scripts;

	std::vector<std::shared_ptr<Script>> _scriptBuffer;
	std::vector<std::list<std::shared_ptr<Script>>::iterator> _terminationBuffer;

	std::unordered_map<std::string, std::list<std::shared_ptr<Script>>::iterator > _scriptMap;
};