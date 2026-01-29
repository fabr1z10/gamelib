#include "gamelib/shadow.h"
#include "gamelib/model.h"
#include "gamelib/node.h"
#include "gamelib/collisionengine.h"

Shadow::Shadow(float angle, float scale, int palette) : Component(), _angle(angle), _scale(scale), _palette(palette) {
}
void Shadow::start() {
	auto* room = Game::instance().getCurrentRoom();

	_collisionEngine = room->getCollisionEngine();
	if (_collisionEngine == nullptr) {
		throw std::runtime_error("Added a controller2D but no collision engine is set in the current room");
	}

	auto * model = dynamic_cast<SpriteModelPal*>(_node->getModel());
	if (model == nullptr) {
		throw std::runtime_error("Shadow component can only be added to nodes with SpriteModelPal model");
	}
	auto shadowModel = std::make_shared<ShadowModelPal>(
		model->getBatch(),
		model,
		_angle,
		_scale,
		_palette
	);
	auto shadowNode = std::make_shared<Node>();
	shadowNode->setModel(shadowModel);
	_node->add(shadowNode);
	_sm = shadowModel.get();

}

void Shadow::update(double) {
	auto pos =_node->getWorldPosition();
	auto result = _collisionEngine->raycastAxis(
		pos,
		-1000,
		2,
		Axis::Y,
		nullptr);
	if (result.hit) {
		float gndY = pos.y - result.distance;
		_sm->setGroundY(gndY);
	}



}