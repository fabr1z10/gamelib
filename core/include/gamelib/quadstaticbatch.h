#pragma once

#include "gamelib/static_batch.h"
#include "gamelib/primitives.h"


class QuadStaticBatch : public StaticBatch<VertexTextureRepeat, QuadPrimitive> {
public:
	QuadStaticBatch(IShader* shader, Camera* cam) : StaticBatch<VertexTextureRepeat, QuadPrimitive>(shader, cam) {
		_prim = GL_TRIANGLES;
		_dynamic = false;
	}


	std::shared_ptr<IModel> getModel(const std::string& id) override {
		return std::make_shared<StaticModel<VertexTextureRepeat, QuadPrimitive>>(this);
	}


	void addQuad(int dir, glm::vec3 position, glm::vec2 size, glm::ivec4 texBounds, glm::vec2 repeat) {
		auto sheetSize = _spriteSheet->getSize();
		float ix = 1.0f / sheetSize.x;
		float iy = 1.0f / sheetSize.y;
		auto tBounds = glm::vec4(texBounds.x * ix, texBounds.y * iy, texBounds.z * ix, texBounds.w * iy);
		VertexTextureRepeat v0, v1, v2, v3;

		if (dir == 0) {
			// XZ plane; normal up
			//_vertices[0].normal = n;
			v0.texBounds = tBounds;
			v0.texCoord = glm::vec2(0.f, repeat.y);
			v0.position = position + glm::vec3(0.f, 0.f, size.y);
			//_vertices[1].normal = n;
			v1.texBounds = tBounds;
			v1.texCoord = glm::vec2(repeat.x, repeat.y);
			v1.position = position + glm::vec3(size.x, 0.f, size.y);
			//_vertices[2].normal = n;
			v2.texBounds = tBounds;
			v2.texCoord = glm::vec2(repeat.x, 0.f);
			v2.position = position + glm::vec3(size.x, 0.f, 0.f);
			//_vertices[3].normal = n;
			v3.texBounds = tBounds;
			v3.texCoord = glm::vec2(0.f, 0.f);
			v3.position = position + glm::vec3(0.f, 0.f, 0.f);
		}
		for (int k = 0; k < QuadPrimitive::indicesPerPrimitive; ++k) {
			_indices.push_back(QuadPrimitive::indexPattern[k] + _nPrimitives * QuadPrimitive::verticesPerPrimitive);
		}
		_vertices.push_back(v0);
		_vertices.push_back(v1);
		_vertices.push_back(v2);
		_vertices.push_back(v3);
		_nPrimitives++;





	}



};