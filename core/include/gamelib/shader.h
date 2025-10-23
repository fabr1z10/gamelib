#pragma once

#include <string>
#include <vector>
#include <unordered_map>
#include <functional>
#include <memory>
#include <GL/glew.h>
#include "glm/glm.hpp"
#include "unordered_map"
#include <gamelib/batch.h>

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

	virtual std::shared_ptr<IBatch> createBatch() = 0;

	virtual void use();

	static std::unordered_map<char, std::pair<GLenum, size_t>> _types;

	void setupVertices();

	[[nodiscard]] GLuint getProgId() const;

protected:
	std::vector<VertexInfo> _vertexFormat;
	GLuint _programId;
	GLsizei _stride;
private:
	bool isIntegerType(GLenum type);
};


template<typename VERTEX, typename PRIMITIVE>
class Shader : public IShader {
public:
	Shader(const std::string_view & vertexCode, const std::string_view& fragmentCode) :
			IShader(vertexCode, fragmentCode, VERTEX::vertexFormat) {}

	std::shared_ptr<IBatch> createBatch() override {
		return std::make_shared<Batch<VERTEX, PRIMITIVE>>(this,1000);
	}


};



inline GLuint IShader::getProgId() const {
	return _programId;
}

