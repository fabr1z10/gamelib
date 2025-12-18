#pragma once

#include "gamelib/batch.h"
#include "gamelib/shader.h"
#include "gamelib/node.h"
#include <glm/gtc/type_ptr.hpp>
#include "gamelib/spritesheet.h"
#include "primitives.h"

// this is for dynamic batches!
template<typename VERTEX, typename PRIMITIVE>
class StaticBatch : public IBatch {
public:
	StaticBatch(IShader* shader, Camera* cam) : IBatch(shader, cam), _nPrimitives(0) {
		_prim = PRIMITIVE::glPrimitive;
		_dynamic = false;
	}

	void setData(std::vector<VERTEX>& data) {
		if (data.size() % PRIMITIVE::verticesPerPrimitive != 0) {
			throw std::runtime_error("StaticBatch::setData: data size is not a multiple of vertices per primitive (" + std::to_string(PRIMITIVE::verticesPerPrimitive) +")");
		}
		_vertices = data;
		_nPrimitives = data.size() / PRIMITIVE::verticesPerPrimitive;
		_indices.reserve(PRIMITIVE::indicesPerPrimitive * _nPrimitives);
		initIndices();

	}

	void configure() override {
		glGenVertexArrays(1, &_vao);
		glBindVertexArray(_vao);

		glGenBuffers(1, &_vbo);
		glBindBuffer(GL_ARRAY_BUFFER, _vbo);
		glBufferData(GL_ARRAY_BUFFER, sizeof(VERTEX) * _vertices.size(), &_vertices[0], GL_STATIC_DRAW);

		VERTEX::setupVertices();


		glGenBuffers(1, &_ebo);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _ebo);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * _indices.size(), &_indices[0], GL_STATIC_DRAW);
	}

	virtual void initIndices() {
		_indices.resize(_nPrimitives * PRIMITIVE::indicesPerPrimitive);
		int base = 0;
		int j = 0;
		for (int i = 0; i < _nPrimitives; ++i) {
			for (int k = 0 ; k < PRIMITIVE::indicesPerPrimitive; ++k) {
				_indices[j++] = base + PRIMITIVE::indexPattern[k];
			}
			base += PRIMITIVE::verticesPerPrimitive;
		}
	}

	void draw() override {
		glBindVertexArray(_vao);
		glDrawElements(_prim, PRIMITIVE::indicesPerPrimitive * _nPrimitives, GL_UNSIGNED_INT, (GLvoid*)0);
		glBindVertexArray(0);
	}

	virtual void initBatch(Node* node) {
		auto vp = _cam->getViewport();
		glViewport(vp.x, vp.y, vp.z, vp.w);

		int jointMatrixLoc = glGetUniformLocation(_shader->getProgramId(), "mvp_mat");
		auto mvpMatrix =  _cam->getProjectionMatrix() * _cam->getViewMatrix() * node->getWorldMatrix();
		glUniformMatrix4fv(jointMatrixLoc, 1, GL_FALSE, glm::value_ptr(mvpMatrix[0]));

		int modelMatrix = glGetUniformLocation(_shader->getProgramId(), "model_mat");
		if (modelMatrix != -1) {
			auto mt = glm::mat3(node->getWorldMatrix()); //glm::transpose(glm::inverse(glm::mat3(node->getWorldMatrix())));
			glUniformMatrix3fv(modelMatrix, 1, GL_FALSE, glm::value_ptr(mt));
		}

		int dirLight = glGetUniformLocation(_shader->getProgramId(), "lightDir");
		if (dirLight != -1) {
			glUniform3fv(dirLight, 1, glm::value_ptr(_lightDirection));
		}


		if (_spriteSheet) {
			_spriteSheet->setupGL(_shader);
		}
	}




	const std::type_info& vertexType() const override {
		return typeid(VERTEX);
	}
protected:

	std::vector<VERTEX> _vertices;
	std::vector<unsigned> _indices;
	size_t _nPrimitives;


};

