#pragma once


#include <GL/glew.h>
#include <glm/glm.hpp>
#include <string>

struct VertexColor {
	glm::vec3 position;
	glm::vec4 color;

	static std::string vertexFormat;

	static void setupVertices() {

		auto stride = sizeof(VertexColor);

		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, stride, 0);

		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, stride, (const void *) offsetof(VertexColor, color));
	}
};

struct VertexColorNormal {
	glm::vec3 position;
	glm::vec4 color;
	glm::vec3 normal;

	static std::string vertexFormat;

	static void setupVertices() {

		auto stride = sizeof(VertexColorNormal);

		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, stride, 0);

		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, stride, (const void *) offsetof(VertexColorNormal, color));

		glEnableVertexAttribArray(2);
		glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, stride, (const void *) offsetof(VertexColorNormal, normal));
	}
};


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

	//static std::string vertexFormat;
	inline static std::string vertexFormat = "3f2f1i";


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

struct VertexTextureRepeat {
	glm::vec3 position;
	glm::vec4 texBounds;
	glm::vec2 texCoord;
	//glm::vec3 normal;
	static std::string vertexFormat;
	static void setupVertices() {
		auto stride = sizeof(VertexTextureRepeat);

		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, stride, 0);

		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, stride, (const void *) offsetof(VertexTextureRepeat, texBounds));

		glEnableVertexAttribArray(2);
		glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, stride, (const void *) offsetof(VertexTextureRepeat, texCoord));

	}
};


struct VertexSkeletal {
public:
	glm::vec3 position;
	glm::vec2 texture;
	glm::vec3 weights;
	static std::string vertexFormat;
	static void setupVertices() {

		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(VertexSkeletal), (void*)0);

		// Define texture coordinate attribute (2 floats per vertex)
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(VertexSkeletal), (void*)(3 * sizeof(float)));

		// Define weights attribute (3 floats per vertex)
		glEnableVertexAttribArray(2);
		glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(VertexSkeletal), (void*)(5 * sizeof(float)));


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