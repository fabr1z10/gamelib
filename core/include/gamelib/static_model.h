#pragma once

#include "gamelib/model.h"
#include "gamelib/static_batch.h"

template<typename VERTEX, typename PRIMITIVE>
class StaticModel : public IModel {
public:
	StaticModel(IBatch* batch) {
		_batch = dynamic_cast<StaticBatch<VERTEX, PRIMITIVE>*>(batch);
		if (_batch == nullptr) {
			throw std::runtime_error("Batch vertex type mismatch! ");
		}

	}

	void draw(IShader* shader) override {
		if (_batch->getShader() == shader) {
			_batch->initBatch(_owner);
			_batch->draw();
		}

	}

protected:
	StaticBatch<VERTEX, PRIMITIVE>* _batch;
};
