#include "gamelib/shader.h"
#include <iostream>

std::unordered_map<char, std::pair<GLenum, size_t>> IShader::_types {
	{'b', {GL_BYTE, sizeof(GLbyte)}},
	{'B', {GL_UNSIGNED_BYTE, sizeof(GLubyte)}},
	{'s', {GL_SHORT, sizeof(GLshort)}},
	{'S', {GL_UNSIGNED_SHORT, sizeof(GLushort)}},
	{'i', {GL_INT, sizeof(GLint)}},
	{'I', {GL_UNSIGNED_INT, sizeof(GLuint)}},
	{'f', {GL_FLOAT, sizeof(GLfloat)}},
	{'d', {GL_DOUBLE, sizeof(GLdouble)}}
};

IShader::IShader(const std::string_view &vertexCode, const std::string_view &fragmentCode,
			   const std::string &vertexFormat) : _programId(0), _stride(0) {

	auto compileShader = [](GLenum type, const char* code, const char* stage) -> GLuint {
		GLuint sid = glCreateShader(type);
		glShaderSource(sid, 1, &code, nullptr);
		glCompileShader(sid);

		GLint ok = 0;
		glGetShaderiv(sid, GL_COMPILE_STATUS, &ok);
		if (!ok) {
			GLint len = 0;
			glGetShaderiv(sid, GL_INFO_LOG_LENGTH, &len);
			std::string log(len, '\0');
			glGetShaderInfoLog(sid, len, nullptr, log.data());
			throw std::runtime_error(std::string("Error compiling ") + stage + " shader:\n" + log);
		}
		return sid;
	};

	GLuint vid = compileShader(GL_VERTEX_SHADER, std::string(vertexCode).c_str(), "vertex");//
	GLuint fid = compileShader(GL_FRAGMENT_SHADER, std::string(fragmentCode).c_str(), "fragment");

	_programId = glCreateProgram();
	glAttachShader(_programId, vid);
	glAttachShader(_programId, fid);
	glLinkProgram(_programId);
	GLint ok;
	glGetProgramiv(_programId, GL_LINK_STATUS, &ok);
	if (!ok) {
		GLint len = 0;
		glGetProgramiv(_programId, GL_INFO_LOG_LENGTH, &len);
		std::string log(len, '\0');
		glGetProgramInfoLog(_programId, len, nullptr, log.data());
		throw std::runtime_error("Error linking program:\n" + log);
	}

	glDeleteShader(vid);
	glDeleteShader(fid);

	// parse vertex format
	size_t start = 0;
	for (size_t i = 0; i < vertexFormat.size(); ++i) {
		if (std::isalpha(vertexFormat[i])) {
			auto size = std::stoi(vertexFormat.substr(start, i-start));
			if (_types.count(vertexFormat[i]) == 0) {
				throw std::runtime_error("Unknown vertex format type: " + std::string(1, vertexFormat[i]));
			}
			const auto& typeInfo = _types.at(vertexFormat[i]);
			_vertexFormat.emplace_back(VertexInfo{size, typeInfo.first, size * (unsigned long)typeInfo.second});
			start = i+1;
			_stride += size * typeInfo.second;
		}
	}
}

IShader::~IShader() {
	glDeleteProgram(_programId);
}

void IShader::use() {
	glUseProgram(_programId);
}

void IShader::setupVertices() {
	GLuint i{0};
	size_t ptr{0};
	for (const auto& vertex : _vertexFormat) {
		glEnableVertexAttribArray(i);
		if (isIntegerType(vertex.type))
			glVertexAttribIPointer(i, vertex.size, vertex.type, _stride, (void*)ptr);
		else
			glVertexAttribPointer(i, vertex.size, vertex.type, GL_FALSE, _stride, (void*)ptr);
		ptr += vertex.byteSize;
		i++;
	}
}


bool IShader::isIntegerType(GLenum type) {
	return type == GL_BYTE || type == GL_UNSIGNED_BYTE ||
		   type == GL_SHORT || type == GL_UNSIGNED_SHORT ||
		   type == GL_INT || type == GL_UNSIGNED_INT;
}

void IShader:: setInt(const std::string &name, int value) const {
	glUniform1i(glGetUniformLocation(_programId, name.c_str()), value);
}
