#include "gamelib/mouselistener.h"
#include "gamelib/game.h"

MouseListener::MouseListener() {
	Game::instance().registerToMouseEvent(this);

}

MouseListener::~MouseListener() {
	Game::instance().unregisterToMouseEvent(this);

}