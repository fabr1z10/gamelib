#include <numeric>
#include "gamelib/text.h"
#include "gamelib/spritesheet.h"
#include "gamelib/font.h"



std::vector<std::string> Text::splitIntoRowsByWords(const std::string& text, size_t n) {
	std::vector<std::string> rows;
	std::istringstream iss(text);
	std::string word;
	std::string currentLine;

	while (iss >> word) {
		if (currentLine.empty()) {
			// first word in the line
			currentLine = word;
		} else if (currentLine.size() + 1 + word.size() <= n) {
			// can fit this word in current line
			currentLine += " " + word;
		} else {
			// cannot fit, push current line and start new
			rows.push_back(currentLine);
			currentLine = word;
		}
	}

	// push the last line
	if (!currentLine.empty())
		rows.push_back(currentLine);

	return rows;
}

//Text::Text(const std::string& font, const std::string &text, Color color, HAlign align, float width, Vec2 anchor) : Node(),
//    _color(color), _hAlign(align), _width(width), _anchor(anchor.x, anchor.y), _palette(-1) {
//
//    //_font = AssetManager::instance().getFont(font).get();
//
//    _lineHeight = _font->getLineHeight();
//    //_width = 0.f;
//
//    updateText(text);
//
//}
Text::Text(IBatch* batch, const std::string& font, const std::string &text, int palette,
	HAlign align, float width, Anchor anchor) : Node(), _batch(batch), _palette(palette), _hAlign(align), _width(width),
	_anchor(anchor), _font(nullptr) {

	_font = batch->getSpriteSheet()->getFont(font);
	_lineHeight = _font->getLineHeight();
	updateText(text);

}

void Text::updateText(const std::string & text) {
	// split string into multiple rows if necessary
	std::string fmtString;
	std::vector<std::string> lines;
	if (_width != 0.f) {
		lines = splitIntoRowsByWords(text, _width);

	} else {
		lines.push_back(text);
	}
	auto model = _font->buildModel(_batch, lines);

	setModel(model);

//	_text = text;
//	auto s32 = Font::getString32(text);
//
//	float crl = 0.f;		// current row length
//	float cel = 0.f;
//	float cwl = 0.f;		// current world length
//	int eol = -1;		// eol index
//	int cws = -1;
//
//	std::vector<TextRow> rows;
//	rows.emplace_back(TextRow(0));
//	int i = 0;
//	for (const auto& c : s32) {
//
//		if ( c == '\n') {
//			rows.back().indexEnd = i;
//			rows.back().length = crl;
//			rows.emplace_back(TextRow(i+1));
//			eol = 0;
//			crl = 0;
//			cel = 0;
//			cwl = 0;
//		} else {
//
//			const auto &charInfo = _font->getCharInfo(c);
//
//			if (c == 0x20) {
//				eol = i;
//				cel = crl;
//				crl += charInfo.advance;
//				cws = -1;
//				cwl = 0;
//			} else {
//				if (cws == -1) cws = i;
//				crl += charInfo.advance;
//				cwl += charInfo.advance;
//				if (_width > 0 && crl > _width) {
//					// the i-th character is beyond width -->
//					if (cwl > _width) {
//						rows.back().indexEnd = i;
//						rows.back().length = _width;
//						// then break the word
//						rows.emplace_back(TextRow(i));
//						crl = charInfo.advance;
//						cwl = charInfo.advance;
//						cel = 0;
//					} else {
//						rows.back().indexEnd = eol;
//						rows.back().length = cel;
//						rows.emplace_back(TextRow(cws));
//						crl = cwl;
//						cel = 0;
//					}
//				}
//			}
//		}
//		i++;
//	}
//	rows.back().length = crl;
//
//
//	// now that we have defined the rows we can create the quads
//
//	std::vector<models::QuadInfo> modelRaw;
//
//	float y = -_lineHeight;
//	float wd{0.f};
//	for (const auto& row : rows) {
//		wd = std::max(wd, row.length);
//	}
//
//	_size = glm::vec2(wd, rows.size() * _lineHeight);
//	glm::vec2 offset (_anchor.x * _size.x, -_anchor.y*_size.y);
//	for (const auto& row : rows) {
//		float x{0.f};
//		if (_hAlign == HAlign::CENTER) {
//			x = (_width - row.length) * 0.5f;
//		} else if (_hAlign == HAlign::RIGHT) {
//			x = (_width - row.length);
//		}
//		int ec = row.indexEnd == -1 ? s32.length() : row.indexEnd;
//		int len = ec - row.indexStart;
//		if (len > 0) {
//			for (int i = row.indexStart; i < ec; ++i) {
//				const auto &c = _font->getCharInfo(s32[i]);
//				glm::vec3 p(x, y, 0.f);
//				//auto kwargs = pybind11::dict("pos"_a=glm::vec3(x,y,0.f),  "size"_a=glm::vec2(c.w,c.h), "normalized"_a=true, "pal"_a=_paletteIndex);
//				int tid = (_palette == -1) ? _font->getTexId() : _palette;
//				models::QuadInfo quad {c.tx, c.tx + c.tw, c.ty, c.ty + c.th, 0, 0,
//									   offset.x + x, offset.y + y, _palette};
////				std::vector<float> charData{
////					c.tx, c.ty, c.tw, c.th,						// tex coords
////                    offset.x - x, offset.y - y,					// anchor
////					_lineHeight * (c.tw/c.th), _lineHeight, 	// size
////					(float)tid									// tex id or palette id
////				};
//				modelRaw.push_back(quad); //modelRaw.end(), charData.begin(), charData.end());
//
//				x += c.advance;
//			}
//		}
//		y -= _lineHeight;
//	}
//
//
//
//
//
//	int rowCount {0};
//	float maxRowLength = 0;
//	for (const auto& row : rows) {
//		maxRowLength = std::max(row.length, maxRowLength);
//		int ec = row.indexEnd == -1 ? text.length() : row.indexEnd;
//		int len = ec - row.indexStart;
//		if (len > 0) {
//			//std::cout << text.substr(row.indexStart, len) << "\n";
//			rowCount++;
//		}
//	}
//	//std::cout << "n rows = " << rowCount << "\n";
//	auto model = std::make_shared<models::TileMap>(_batch->getId(), _lineHeight, _lineHeight, modelRaw);
//
//	this->setModel(model); //, _font->getBatchId());
//	//this->setMultiplyColor(_color);
//	_model->draw();





}