#pragma once

#include "gamelib/shader.h"
#include "gamelib/dynamic_batch.h"
#include "gamelib/static_batch.h"

template<typename VERTEX, typename PRIMITIVE>
class Shader : public IShader {
public:
	Shader(const std::string_view & vertexCode, const std::string_view& fragmentCode) :
			IShader(vertexCode, fragmentCode, VERTEX::vertexFormat) {}

	std::shared_ptr<IBatch> createBatch(Camera* cam, int n = 1000) override {
		if (n == 0) {
			return std::make_shared<StaticBatch<VERTEX, PRIMITIVE>>(this, cam);
		} else {
			return std::make_shared<Batch<VERTEX, PRIMITIVE>>(this, cam, n);
		}
	}
};



