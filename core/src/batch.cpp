#include "gamelib/batch.h"
#include "gamelib/shader.h"
#include "gamelib/spritesheet.h"

IBatch::IBatch(IShader* shader, Camera* cam) : _shader(shader), _cam(cam), _nPrimitive(0) {
}

void IBatch::addSpriteSheet(std::shared_ptr<SpriteSheet> sheet) {
	_spriteSheet = sheet;
}

void IBatch::startShader() {
	_shader->use();
}

int IBatch::getPrimitiveId() {
	int next;
	if (_deallocated.empty()) {
		next = _nPrimitive;
		_nPrimitive++;
	} else {
		next = _deallocated.front();
		_deallocated.pop_front();
	}
	return next;
}

void IBatch::releasePrimitiveId(int id) {
	_deallocated.push_back(id);
}

std::shared_ptr<IModel> IBatch::getModel(const std::string &id) {
	return _spriteSheet->getModel(this, id);
}

bool IBatch::isDynamic() const {
	return _dynamic;
}