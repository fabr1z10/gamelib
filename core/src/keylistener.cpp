#include "gamelib/keylistener.h"
#include "gamelib/game.h"


KeyListener::KeyListener() {
	Game::instance().registerToKeyboardEvent(this);
}

KeyListener::~KeyListener() {
	Game::instance().unregisterToKeyboardEvent(this);
}