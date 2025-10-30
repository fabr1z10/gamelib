#include <GL/glew.h>
#include <glm/glm.hpp>
#include <string>

struct VertexTexture {
	glm::vec3 position;
	glm::vec2 texCoord;

	static std::string vertexFormat;

	static void setupVertices() {

		auto stride = sizeof(VertexTexture);

		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, stride, 0);

		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, stride, (const void *) offsetof(VertexTexture, texCoord));
	}
};

struct VertexTexturePalette {
	glm::vec3 position;
	glm::vec2 texCoord;
	int paletteId;

	static std::string vertexFormat;

	static void setupVertices() {

		auto stride = sizeof(VertexTexturePalette);

		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, stride, 0);

		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, stride, (const void *) offsetof(VertexTexturePalette, texCoord));

		glEnableVertexAttribArray(2);
		glVertexAttribIPointer(2, 1, GL_INT, stride, (const void *) offsetof(VertexTexturePalette, paletteId));


	}
};


struct LinePrimitive {
	static constexpr GLenum glPrimitive = GL_LINES;
	static constexpr int verticesPerPrimitive = 2;
	static constexpr int indicesPerPrimitive = 2;
	static constexpr unsigned indexPattern[2] = {0,1};
};

struct TrianglePrimitive {
	static constexpr GLenum glPrimitive = GL_TRIANGLES;
	static constexpr int verticesPerPrimitive = 3;
	static constexpr int indicesPerPrimitive = 3;
	static constexpr unsigned indexPattern[3] = {0, 1, 2};
};

struct QuadPrimitive {
	static constexpr GLenum glPrimitive = GL_TRIANGLES;
	static constexpr int verticesPerPrimitive = 4;
	static constexpr int indicesPerPrimitive = 6;
	static constexpr unsigned indexPattern[6] = {0, 1, 2, 0, 2, 3};
};