#include <cmath>

#include "gamelib/model.h"
#include "gamelib/node.h"

glm::vec3 IModel::getWorldPosition() const {
	if (_owner) {
		return _owner->getWorldPosition();
	} else {
		return glm::vec3(0);
	}
}

void IModel::setOnLoopEnd(const std::function<void()> &callback) {
	_onLoopEnd = callback;
}

Transform IModel::getWorldTransform() const {
	if (_owner) {
		return _owner->getWorldTransform();
	} else {
		return Transform{};
	}
}

QuadModelRepeat::QuadModelRepeat(IBatch* b, const QuadInfo& info, glm::ivec4 texBounds)
		: QuadModelBase(b, info), _texBounds(texBounds) {

	auto spriteSheetSize = b->getSpriteSheet()->getSize();
	auto ix = 1.f / spriteSheetSize.x;
	auto iy = 1.f / spriteSheetSize.y;
	_texBounds = glm::vec4(texBounds.x * ix, texBounds.y * iy, texBounds.z * ix, texBounds.w * iy);



}

void QuadModelRepeat::setVertex(VertexTextureRepeat* v, const glm::vec3& pos, const glm::vec2& uv)  {
	v->position = pos;
	v->texCoord = uv;
	v->texBounds = _texBounds;
}


ShadowModelPal::ShadowModelPal(IBatch* batch, SpriteModelPal *reference, float angle, float scale, int pal) :
	SpriteBase<VertexTexturePalette>(batch, reference->getSpriteInfo()), _reference(reference), _pal(pal), _gndY(0.f) {
	float radAngle = glm::radians(angle);
	_versor = scale * glm::vec3(std::cos(radAngle), std::sin(radAngle), 0.0f);
}
