
#include "gamelib/game.h"
#include "factory.h"


int main() {


	Game& g = Game::instance();
	g.init("..");
	auto factory = std::make_shared<RoomFactory>();
	g.setRoomFactory(factory);
	g.run();
}

