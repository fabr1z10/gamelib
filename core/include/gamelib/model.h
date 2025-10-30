#pragma once

#include <vector>
#include "gamelib/room.h"
#include "gamelib/game.h"
#include "gamelib/primitives.h"
#include "gamelib/dynamic_batch.h"
#include "gamelib/util.h"
#include "gamelib/spritesheet.h"

class Node;

class IModel {

public:
	// this will be called once per frame to update the model's vertices (for instance sprite frame change)
	virtual void update() {
		// default: do nothing
	}

	// this is to be called whenever position or other stuff changes that requires a full refresh of the model
	virtual void refresh() = 0;

	glm::vec3 getWorldPosition() const;

	void setOwner(Node*);
protected:
	Node* _owner = nullptr;
};

inline void IModel::setOwner(Node * node) {
	_owner = node;
}

template<typename VERTEX, typename PRIMITIVE>
class Model : public IModel {
public:
	Model(IBatch* batch) {
		_batch = dynamic_cast<Batch<VERTEX, PRIMITIVE>*>(batch);
		if (_batch == nullptr) {
			throw std::runtime_error("Batch vertex type mismatch! ");
		}

	}

	virtual ~Model() {
		for (const auto& i : _primitives) _batch->releasePrimitiveId(i);
		_vertices.clear();
	}

	void allocate(int primitives) {
		for (size_t i = 0; i < primitives; ++i) {
			int np = _batch->getPrimitiveId();
			_primitives.push_back(np);
			_vertices.push_back(_batch->getAddress(np));
		}
	}


protected:
	Batch<VERTEX, PRIMITIVE>* _batch;
	std::vector<VERTEX*> _vertices;
	std::vector<int> _primitives;
};

template<typename T>
class QuadModelBase : public Model<T, QuadPrimitive> {
public:
	QuadModelBase(IBatch* batch, const QuadInfo& info) :
			Model<T, QuadPrimitive>(batch), _size(info.width, info.height), _quadInfo(info) {
		this->allocate(1);
	}

	void refresh() override {
		auto pos = this->getWorldPosition();
		pos.x -= _quadInfo.anchorX;
		pos.y -= _quadInfo.anchorY;
		// vert 0 is bottom left
		T* p = this->_vertices[0];
		float tx0 = _quadInfo.tx0;;
		float ty0 = _quadInfo.ty0;
		float tx1 = _quadInfo.tx1;;
		float ty1 = _quadInfo.ty1;
		if (_quadInfo.flipx) {
			std::swap(tx0, tx1);
		}
		if (_quadInfo.flipy) {
			std::swap(ty0, ty1);
		}
		setVertex(p + 0, pos + glm::vec3(0, 0, 0), glm::vec2(tx0, ty1));
		setVertex(p + 1, pos + glm::vec3(_size.x, 0, 0), glm::vec2(tx1, ty1));
		setVertex(p + 2, pos + glm::vec3(_size.x, _size.y, 0), glm::vec2(tx1, ty0));
		setVertex(p + 3, pos + glm::vec3(0, _size.y, 0), glm::vec2(tx0, ty0));

	}

	virtual void setVertex(T* v, const glm::vec3& pos, const glm::vec2& uv) {
		v->position = pos;
		v->texCoord = uv;
	}




protected:
	const QuadInfo& _quadInfo;
	glm::vec2 _size;
};

class QuadModel : public QuadModelBase<VertexTexture> {
public:
	QuadModel(IBatch* b, const QuadInfo& info)
			: QuadModelBase(b, info) {}
};

class QuadModelPal : public QuadModelBase<VertexTexturePalette> {
public:
	QuadModelPal(IBatch* b, const QuadInfo& info)
		: QuadModelBase(b, info) {}

	// 2️⃣ Specialize setVertex *only for this vertex type*
	void setVertex(VertexTexturePalette* v, const glm::vec3& pos, const glm::vec2& uv) override {
		v->position = pos;
		v->texCoord = uv;
		v->paletteId = _quadInfo.palette;
	}
private:

	int _pal;
};


template<typename T>
class SpriteBase : public Model<T, QuadPrimitive> {
public:
	SpriteBase(IBatch* batch, const SpriteInfo& info) :
			Model<T, QuadPrimitive>(batch), _spriteInfo(info) {
		_animation = info.getDefaultAnimation();
		_frame = 0;
		_ticks = 0;
		this->allocate(1);
	}

	void update() override {
		const Frame& frame = _spriteInfo.getFrame(_animation, _frame);
		_ticks++;
		if (_ticks >= frame.ticks) {
			_ticks = 0;
			_frame = _spriteInfo.next(_animation,  _frame);
			this->refresh();
		}
	}

	void refresh() override {
		auto pos = this->getWorldPosition();
		const Frame& frame = _spriteInfo.getFrame(_animation, _frame);
		const QuadInfo& info = frame.quad;
		// vert 0 is bottom left
		T* p = this->_vertices[0];
		float tx0 = info.tx0;;
		float ty0 = info.ty0;
		float tx1 = info.tx1;;
		float ty1 = info.ty1;
		if (info.flipx) {
			std::swap(tx0, tx1);
		}
		if (info.flipy) {
			std::swap(ty0, ty1);
		}
		this->setVertex(p + 0, pos + glm::vec3(0, 0, 0), glm::vec2(tx0, ty1));
		this->setVertex(p + 1, pos + glm::vec3(info.width, 0, 0), glm::vec2(tx1, ty1));
		this->setVertex(p + 2, pos + glm::vec3(info.width, info.height, 0), glm::vec2(tx1, ty0));
		this->setVertex(p + 3, pos + glm::vec3(0, info.height, 0), glm::vec2(tx0, ty0));
	}

	virtual void setVertex(T* v, const glm::vec3& pos, const glm::vec2& uv) {
		v->position = pos;
		v->texCoord = uv;
	}

protected:
	std::string _animation;
	int _frame;
	int _ticks;
	const SpriteInfo& _spriteInfo;

};

class SpriteModel : public SpriteBase<VertexTexture> {
public:
	SpriteModel(IBatch* b, const SpriteInfo& info)
			: SpriteBase(b, info) {}
};

class SpriteModelPal : public SpriteBase<VertexTexturePalette> {
public:
	SpriteModelPal(IBatch* b, const SpriteInfo& info)
			: SpriteBase(b, info) {}

	// 2️⃣ Specialize setVertex *only for this vertex type*
	void setVertex(VertexTexturePalette* v, const glm::vec3& pos, const glm::vec2& uv) override {
		v->position = pos;
		v->texCoord = uv;
		v->paletteId = this->_spriteInfo.getFrame(_animation, _frame).quad.palette;
	}
private:

	int _pal;
};