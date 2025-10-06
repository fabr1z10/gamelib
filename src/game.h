#pragma once

#include <memory>
#include <string>

class Game {
public:
	static Game& instance() {
		if (!_instance) {
			_instance.reset(new Game());
		}
		return *_instance;
	}
private:
	Game();
	std::string detectGameDir();
	static std::unique_ptr<Game> _instance;
};