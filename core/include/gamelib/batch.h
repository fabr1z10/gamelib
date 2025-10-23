#pragma once

#include <GL/glew.h>
#include <vector>

class IShader;

class IBatch {
public:
	IBatch(IShader* shader);

	virtual ~IBatch() = default;

	virtual void configure() = 0;

	virtual void draw() = 0;

	virtual void startShader();
protected:
	GLuint _vao;
	GLuint _vbo;
	GLuint _ebo;
	// primitive type used for rendering
	GLenum _prim;
	IShader* _shader;
};

template<typename VERTEX, typename PRIMITIVE>
class Batch : public IBatch {
public:
	Batch(IShader* shader, size_t maxPrimitives) : IBatch(shader), _maxPrimitives(maxPrimitives), _nPrimitive(0) {
		_vertices.reserve(PRIMITIVE::verticesPerPrimitive * maxPrimitives);
		_indices.reserve(PRIMITIVE::indicesPerPrimitive * maxPrimitives);
		_prim = PRIMITIVE::glPrimitive;
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

		glBindBuffer(GL_ARRAY_BUFFER, _vbo);
		glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(VERTEX) * _nPrimitive * PRIMITIVE::verticesPerPrimitive, &_vertices[0]);

		//initDraw(s);
		glBindVertexArray(_vao);
		glDrawElements(_prim, PRIMITIVE::indicesPerPrimitive * _nPrimitive, GL_UNSIGNED_INT, (GLvoid*)0);
		glBindVertexArray(0);
	}
private:

	std::vector<VERTEX> _vertices;
	std::vector<unsigned> _indices;
	size_t _maxPrimitives;
	int _nPrimitive;		// holds the # of primitives stored!

};
