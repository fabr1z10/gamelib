#pragma once

#include <string>
#include <GL/glew.h>   // or glad/gl.h depending on your setup
#include <unordered_map>
#include <glm/glm.hpp>
#include <memory>
#include <vector>

class Palette {

public:
	Palette(const std::string& name);

	//glm::ivec4 getColor(int index, const glm::ivec4& defaultColor) const;

	void addColor(int, glm::ivec4);

	const std::unordered_map<int, glm::ivec4>& getColors() const;

private:
	std::string _name;
	std::unordered_map<int, glm::ivec4> _colors;
};

inline const std::unordered_map<int, glm::ivec4> &Palette::getColors() const {
	return _colors;
}

class Tex {
public:
	enum class Format {
		RGB,
		RGBA,
		INDEXED,  // paletted
		GRAYSCALE
	};
	explicit Tex(const std::string& filename);

	Tex() = default;

	~Tex();

	void load(const std::string& filename,
			  std::vector<Palette>* = nullptr);

	GLuint getId() const;

	GLuint getPaletteId() const;

	static std::shared_ptr<Tex> getTexture(const std::string& filename);

	glm::ivec2 getSize() const;

	Format getFormat() const {
		return _format;
	}
private:

	static std::unordered_map<std::string, std::shared_ptr<Tex>> _textureStore;
	int _width = 0;
	int _height = 0;
	Format _format = Format::RGBA;
	GLuint _texId = 0;
	GLuint _paletteId = 0;

};

inline GLuint Tex::getId() const {
	return _texId;
}

inline GLuint Tex::getPaletteId() const {
	return _paletteId;
}

inline glm::ivec2 Tex::getSize() const {
	return {_width, _height};
}