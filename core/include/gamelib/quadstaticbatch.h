#pragma once

#include "gamelib/static_batch.h"
#include "gamelib/primitives.h"


class QuadStaticBatch : public StaticBatch<VertexTextureNormal, QuadPrimitive> {
public:
	QuadStaticBatch(IShader* shader, Camera* cam) : StaticBatch<VertexTextureNormal, QuadPrimitive>(shader, cam) {
		_prim = GL_TRIANGLES;
		_dynamic = false;
	}

	void addQuad(int dir, glm::vec3 position, glm::vec2 size, glm::ivec4 texBounds, glm::vec2 repeat) {
		auto sheetSize = _spriteSheet->getSize();
		float ix = 1.0f / sheetSize.x;
		float iy = 1.0f / sheetSize.y;
		auto tBounds = glm::vec4(texBounds.x * ix, texBounds.y * iy, texBounds.z * ix, texBounds.w * iy);
		if (dir == 0) {
			glm::vec3 n = glm::vec3(0.f, 0.f, 1.f);
			// XZ plane
			_vertices[0].position = position;
			_vertices[0].normal = n;
			_vertices[0].texBounds = texBounds;
			_vertices[0].texCoords = glm::vec2(0.f, repeat.y);
			_vertices[1].position = position + glm::vec3(size.x, 0.f, 0.f);
			_vertices[1].normal = n;
			_vertices[1].texBounds = texBounds;
			_vertices[1].texCoords = glm::vec2(repeat.x, repeat.y);
			_vertices[2].position = position + glm::vec3(size.x, 0.f, size.y);
			_vertices[2].normal = n;
			_vertices[2].texBounds = texBounds;
			_vertices[2].texCoords = glm::vec2(repeat.x, 0.f);
			_vertices[3].position = position + glm::vec3(0.f, 0.f, size.y);
			_vertices[3].normal = n;
			_vertices[3].texBounds = texBounds;
			_vertices[3].texCoords = glm::vec2(0.f, 0.f);
		}





	}



};