#include "gamelib/collider.h"
#include "gamelib/game.h"
#include "gamelib/node.h"
#include "gamelib/collisionengine.h"



BasicCollider::BasicCollider(std::shared_ptr<Shape> shape, int flag, int mask, const std::string& tag) :
		Collider(), _shape(shape), _flag(flag), _mask(mask), _tag(tag) {

}

Collider::~Collider() {
	auto* engine = Game::instance().getCurrentRoom()->getCollisionEngine();
	engine->rmCollider(this);
}

void Collider::start() {
	auto* room = Game::instance().getCurrentRoom();
	auto* dbgBatch = room->getBatch("debug-line");

	auto* engine = room->getCollisionEngine();
	if (engine == nullptr) {
		throw std::runtime_error("Added a collider but no collision engine is set in the current room");
	}

	engine->addCollider(this);

	if (dbgBatch != nullptr) {
		auto model = getShape()->makeModel(dbgBatch, glm::vec4(1.f), ModelType::WIREFRAME);
		auto node = std::make_shared<Node>();
		node->setModel(model);
		_node->add(node);
	}
}


Bounds Collider::getBounds() {
	auto local = getShape()->getBounds();
	if (_node == nullptr) {
		return local;
	}
	return _node->transformBounds(local);

}


Shape* BasicCollider::getShape() {
	return _shape.get();
}

int BasicCollider::getCollisionFlag() const {
	return _flag;
}

int BasicCollider::getCollisionMask() const {
	return _mask;
}

std::string BasicCollider::getTag() const {
	return _tag;
}