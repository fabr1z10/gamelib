#pragma once

#include <unordered_map>
#include <yaml-cpp/yaml.h>
#include "gamelib/batch.h"

class SpriteSheet;

struct CharInfo {
	// texture top left
	float tx, ty;
	// size
	float tw, th;
	// width and height in px
	float w, h;
	float advance;
	float oy;
};


class Font {
public:
	Font(const YAML::Node& node, SpriteSheet* batch);

	float getLineHeight() const;

	static std::u32string getString32(const std::string&);

	// TODO create a model
	virtual std::shared_ptr<IModel> buildModel(IBatch*, const std::vector<std::string>&, int pal) const = 0;

	static std::shared_ptr<Font> makeFont(const YAML::Node& node, SpriteSheet* b);
protected:
	float _lineHeight;
	SpriteSheet* _sheet;
};



inline float Font::getLineHeight() const {
	return _lineHeight;
}

class GenericFont : public Font {
public:
	GenericFont(const YAML::Node& node, SpriteSheet* batch);

	const CharInfo& getCharInfo(char32_t c) const;

	std::shared_ptr<IModel> buildModel(IBatch*, const std::vector<std::string>&, int) const override;

private:
	std::unordered_map<char32_t, CharInfo> m_info;

};

inline const CharInfo& GenericFont::getCharInfo(char32_t c) const {
	return m_info.at(c);
}

// monospaced font --- every character has an index that corresponds to a TILE
class MonospacedFont : public Font {
public:
	MonospacedFont(const YAML::Node& node, SpriteSheet*);

	std::shared_ptr<IModel> buildModel(IBatch*, const std::vector<std::string>&, int) const override;

private:
	std::unordered_map<char32_t, int> _info;
};