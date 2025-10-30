#include "gamelib/model.h"
#include "gamelib/node.h"

glm::vec3 IModel::getWorldPosition() const {
	if (_owner) {
		return _owner->getWorldPosition();
	} else {
		return glm::vec3(0);
	}
}
//QuadModel::QuadModel(const std::string &batchId, int x, int y, int w, int h) :
//	Model<VertexTexture, QuadPrimitive>(batchId), _size(w, h) {
//	allocate(1);
//	auto texSize = _batch->getTexture()->getSize();
//	_tx0 = static_cast<float>(x) / texSize.x;
//	_ty0 = static_cast<float>(y) / texSize.y;
//	_tx1 = static_cast<float>(x+w) / texSize.x;
//	_ty1 = static_cast<float>(y+h) / texSize.y;
//}
//
//void QuadModel::update() {
//	auto pos = _owner->getWorldPosition();
//	// vert 0 is bottom left
//	VertexTexture* p = _vertices[0];
//	p->position = pos + glm::vec3(0);
//	p->texCoord = glm::vec2(_tx0, _ty1);
//	// vert 1 is bottom right
//	(p+1)->position = pos + glm::vec3(_size.x, 0, 0);
//	(p+1)->texCoord = glm::vec2(_tx1, _ty1);
//	// vert 2 is top right
//	(p+2)->position = pos + glm::vec3(_size.x, _size.y, 0);
//	(p+2)->texCoord = glm::vec2(_tx1, _ty0);
//	// vert 3 is top left
//	(p+3)->position = pos + glm::vec3(0, _size.y, 0);
//	(p+3)->texCoord = glm::vec2(_tx0, _ty0);
//}
//
//QuadModelPal::QuadModelPal(const std::string &batchId, int x, int y, int w, int h, int pal) :
//		Model<VertexTexturePalette, QuadPrimitive>(batchId), _size(w, h), _pal(pal){
//	allocate(1);
//	auto texSize = _batch->getTexture()->getSize();
//	_tx0 = static_cast<float>(x) / texSize.x;
//	_ty0 = static_cast<float>(y) / texSize.y;
//	_tx1 = static_cast<float>(x+w) / texSize.x;
//	_ty1 = static_cast<float>(y+h) / texSize.y;
//}
//
//void QuadModelPal::update() {
//	auto pos = _owner->getWorldPosition();
//	// vert 0 is bottom left
//	VertexTexturePalette* p = _vertices[0];
//	p->position = pos + glm::vec3(0);
//	p->texCoord = glm::vec2(_tx0, _ty1);
//	p->paletteId = _pal;
//	// vert 1 is bottom right
//	(p+1)->position = pos + glm::vec3(_size.x, 0, 0);
//	(p+1)->texCoord = glm::vec2(_tx1, _ty1);
//	(p+1)->paletteId = _pal;
//	// vert 2 is top right
//	(p+2)->position = pos + glm::vec3(_size.x, _size.y, 0);
//	(p+2)->texCoord = glm::vec2(_tx1, _ty0);
//	(p+2)->paletteId = _pal;
//
//	// vert 3 is top left
//	(p+3)->position = pos + glm::vec3(0, _size.y, 0);
//	(p+3)->texCoord = glm::vec2(_tx0, _ty0);
//	(p+3)->paletteId = _pal;
//
//}
//
