#pragma once

#include "node.h"
#include "font.h"



// align each row left (default), center, or right w.r.t. box
// this only works if a width is specified

enum class HAlign {
	LEFT = 0,
	CENTER = 1,
	RIGHT = 2
};

enum class Anchor {
	TOP_LEFT, TOP, TOP_RIGHT, LEFT, CENTER, RIGHT, BOTTOM_LEFT, BOTTOM, BOTTOM_RIGHT
};


class Text : public Node {
public:
	Text(IBatch* batch, const std::string& font, const std::string& text, int palette,
		 HAlign align, float width = 0.f, Anchor anchor = Anchor::TOP_LEFT);

	void updateText(const std::string&);

	std::string getText() const;

	glm::vec2 getSize() const;
private:
	std::vector<std::string> splitIntoRowsByWords(const std::string& text, size_t n);

	struct TextRow {
		TextRow (int iStart) : indexStart(iStart), indexEnd(-1), length(0.f) {}
		int indexStart;
		int indexEnd;
		float length;
	};
	float _width;
	float _lineHeight;
	const Font* _font;
	int _palette;
	std::string _palId;
	glm::vec2 _size;
	glm::vec3 _offset;
	int _lines;
	int _paletteIndex;
	HAlign _hAlign;
	Anchor _anchor;
	//std::string _sheetId;
	std::string _text;
	//Color _color;
	//QuadBatchPalette* _batch;
	IBatch* _batch;

};

inline std::string Text::getText() const {
	return _text;
}

inline glm::vec2 Text::getSize() const {
	return glm::vec2(_size.x, _size.y);
}