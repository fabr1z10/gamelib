#pragma once

#include "gamelib/batch.h"
#include "gamelib/shader.h"
#include <glm/gtc/type_ptr.hpp>
#include "gamelib/spritesheet.h"

// this is for dynamic batches!
template<typename VERTEX, typename PRIMITIVE>
class Batch : public IBatch {
public:
	Batch(IShader* shader, Camera* cam, size_t maxPrimitives) : IBatch(shader, cam), _maxPrimitives(maxPrimitives) {
		_vertices.resize(PRIMITIVE::verticesPerPrimitive * maxPrimitives);
		_indices.reserve(PRIMITIVE::indicesPerPrimitive * maxPrimitives);
		_prim = PRIMITIVE::glPrimitive;
	}

	// returns the start address of the i-th primitive's vertex data
	VERTEX* getAddress(int i) {
		return &_vertices[i * PRIMITIVE::verticesPerPrimitive];
	}

	void configure() override {
		glGenVertexArrays(1, &_vao);
		glBindVertexArray(_vao);

		glGenBuffers(1, &_vbo);
		glBindBuffer(GL_ARRAY_BUFFER, _vbo);
		glBufferData(GL_ARRAY_BUFFER, sizeof(VERTEX) * _vertices.size(), &_vertices[0], GL_DYNAMIC_DRAW);

		VERTEX::setupVertices();

		initIndices();

		glGenBuffers(1, &_ebo);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _ebo);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * _indices.size(), &_indices[0], GL_STATIC_DRAW);
	}

	virtual void initIndices() {
		_indices.resize(_maxPrimitives * PRIMITIVE::indicesPerPrimitive);
		int base = 0;
		int j = 0;
		for (int i = 0; i < _maxPrimitives; ++i) {
			for (int k = 0 ; k < PRIMITIVE::indicesPerPrimitive; ++k) {
				_indices[j++] = base + PRIMITIVE::indexPattern[k];
			}
			base += PRIMITIVE::verticesPerPrimitive;
		}
	}

	void draw() override {
		startShader();

		setupUniforms();

		glBindBuffer(GL_ARRAY_BUFFER, _vbo);
		glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(VERTEX) * _nPrimitive * PRIMITIVE::verticesPerPrimitive, &_vertices[0]);

		//initDraw(s);
		glBindVertexArray(_vao);
		glDrawElements(_prim, PRIMITIVE::indicesPerPrimitive * _nPrimitive, GL_UNSIGNED_INT, (GLvoid*)0);
		glBindVertexArray(0);
	}

	void setupUniforms() override {
		auto vp = _cam->getViewport();
		glViewport(vp.x, vp.y, vp.z, vp.w);
		int jointMatrixLoc = glGetUniformLocation(_shader->getProgramId(), "pv_mat");
		auto pvMatrix =  _cam->getProjectionMatrix() * _cam->getViewMatrix();
		glUniformMatrix4fv(jointMatrixLoc, 1, GL_FALSE, glm::value_ptr(pvMatrix[0]));

		if (_spriteSheet) {

			_spriteSheet->setupGL(_shader);
		}

	}

	const std::type_info& vertexType() const override {
		return typeid(VERTEX);
	}
private:

	std::vector<VERTEX> _vertices;
	std::vector<unsigned> _indices;
	size_t _maxPrimitives;

};
