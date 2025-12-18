#pragma once

#include <string>
#include <vector>
#include <unordered_map>
#include <functional>
#include <memory>
#include <GL/glew.h>
#include "glm/glm.hpp"
#include "unordered_map"
#include "gamelib/camera.h"


class IBatch;

struct VertexInfo {
	int size;
	GLenum type;
	unsigned long byteSize;
};

class IShader {
public:
	IShader(const std::string_view &vertexCode, const std::string_view &fragmentCode,
		const std::string &vertexFormat);

	virtual ~IShader();

	virtual std::shared_ptr<IBatch> createBatch(Camera*, int) { return nullptr; }

	virtual void use();


	static std::unordered_map<char, std::pair<GLenum, size_t>> _types;

	void setupVertices();

	GLuint getProgramId() const;

	void setInt(const std::string &name, int value) const;
protected:
	std::vector<VertexInfo> _vertexFormat;
	GLuint _programId;
	GLsizei _stride;
private:
	bool isIntegerType(GLenum type);
};

inline GLuint IShader::getProgramId() const {
	return _programId;
}



