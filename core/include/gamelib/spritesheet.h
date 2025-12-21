#pragma once

#include "gamelib/tex.h"
#include <vector>
#include "gamelib/util.h"
#include "yaml-cpp/yaml.h"
#include "gamelib/font.h"


class IModel;
class IShader;
class IBatch;

class SpriteSheet {
public:
	SpriteSheet(const std::string& file);

	static std::shared_ptr<SpriteSheet> getSpriteSheet(const std::string& filename);

	std::shared_ptr<IModel> getModel(IBatch*, const std::string& id);

	bool isIndexed() const;

	void setupGL(IShader* shader) const;

	glm::ivec2 getSize() const;

	const Font* getFont(const std::string&) const;

	int getTileSize() const;

	// get tex coords of tile n
	glm::vec4 getTexCoords(int n) const;
private:
	void loadFonts(const YAML::Node & node);
	static std::unordered_map<std::string, std::shared_ptr<SpriteSheet>> _sheets;
	std::shared_ptr<Tex> _tex;
	std::unordered_map<std::string, int> _paletteNameToIndex;
	std::vector<Palette> _palettes;
	std::unordered_map<std::string, QuadInfo> _quads;
	std::unordered_map<std::string, SpriteInfo> _sprites;
	std::unordered_map<std::string, std::shared_ptr<Font>> _fonts;
	int _tileSize;
};

inline glm::ivec2 SpriteSheet::getSize() const {
	return _tex->getSize();
}

inline const Font *SpriteSheet::getFont(const std::string & id) const {
	return _fonts.at(id).get();
}

inline int SpriteSheet::getTileSize() const {
	return _tileSize;
}