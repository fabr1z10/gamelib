
#include "gamelib/game.h"
#include "factory.h"


int main() {

	auto factory = std::make_shared<RoomFactory>();

	Game& g = Game::instance();
	g.init("..");
	g.setRoomFactory(factory);
	g.run();
}

