#pragma once

#include <vector>
#include "gamelib/room.h"
#include "gamelib/game.h"
#include "gamelib/primitives.h"
#include "gamelib/dynamic_batch.h"
#include "gamelib/util.h"
#include "gamelib/spritesheet.h"
#include "gamelib/transform.h"

class Node;

class IModel {

public:
	// this will be called once per frame to update the model's vertices (for instance sprite frame change)
	virtual void update() {
		// default: do nothing
	}

	virtual void draw(IShader*) {
		// default: do nothing
	}

	// this is to be called whenever position or other stuff changes that requires a full refresh of the model
	virtual void refresh() {}

	glm::vec3 getWorldPosition() const;

	Transform getWorldTransform() const;

	virtual void setAnimation(const std::string&) {}

	std::string getAnimation() { return _animation; }

	void setOwner(Node*);
protected:
	std::string _animation;
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


class LineModel : public Model<VertexColor, LinePrimitive> {
public:
	LineModel(IBatch *b, const std::vector<glm::vec2> &points, const glm::vec4 color, bool closed)
			: Model<VertexColor, LinePrimitive>(b), _points(points), _color(color), _closed(closed) {
		this->allocate(points.size() - (closed ? 0 : 1));
		this->refresh();
	}

	void refresh() override {
		size_t u = _points.size();
		glm::vec3 pos = this->getWorldPosition();
		for (size_t i = 0; i < _points.size() - (_closed ? 0 : 1); ++i) {
			auto p0 = pos + glm::vec3(_points[i], 0);
			auto p1 = pos + glm::vec3(_points[(i + 1) % u], 0);
			VertexColor *v = this->_vertices[i];
			v[0].position = p0;
			v[0].color = _color;
			v[1].position = p1;
			v[1].color = _color;
		}
	}

private:
	std::vector<glm::vec2> _points;
	glm::vec4 _color;
	bool _closed;
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
		this->_animation = info.getDefaultAnimation();
		_frame = 0;
		_ticks = 0;
		this->allocate(1);
	}

	void update() override {
		const Frame& frame = _spriteInfo.getFrame(this->_animation, _frame);
		_ticks++;
		if (_ticks >= frame.ticks) {
			_ticks = 0;
			_frame = _spriteInfo.next(this->_animation,  _frame);
			this->refresh();
		}
	}

	void refresh() override {
		auto t = this->getWorldTransform();
		//std::cout << t.position.x << ", " << t.position.y << ", " << t.position.z << std::endl;

		//auto pos = this->getWorldPosition();
		const Frame& frame = _spriteInfo.getFrame(this->_animation, _frame);
		const QuadInfo& info = frame.quad;
		// sx includes flip
		float sx = t.scale * t.flipX;   // flipX is +1 or -1
		float sy = t.scale;             // assume no flipY; use t.flipY if you have it

		glm::vec3 bottomLeft = t.position - glm::vec3(info.anchorX * sx, info.anchorY * sy, 0.0f);

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
		this->setVertex(p + 0, bottomLeft, glm::vec2(tx0, ty1));
		this->setVertex(p + 1, bottomLeft + glm::vec3(sx * info.width, 0, 0), glm::vec2(tx1, ty1));
		this->setVertex(p + 2, bottomLeft + glm::vec3(sx * info.width, sy * info.height, 0), glm::vec2(tx1, ty0));
		this->setVertex(p + 3, bottomLeft + glm::vec3(0, sy * info.height, 0), glm::vec2(tx0, ty0));
	}

	virtual void setVertex(T* v, const glm::vec3& pos, const glm::vec2& uv) {
		v->position = pos;
		v->texCoord = uv;
	}

protected:

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