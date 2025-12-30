#include "gamelib/font.h"
#include <locale>
#include <codecvt>
#include "gamelib/tilegrid.h"

std::shared_ptr<Font> Font::makeFont(const YAML::Node &node, SpriteSheet *batch) {
	auto fontType = node["type"].as<std::string>();
	if (fontType == "mono") {
		return std::make_shared<MonospacedFont>(node, batch);
	} else {
		return std::make_shared<GenericFont>(node, batch);
	}
}

std::shared_ptr<IModel> GenericFont::buildModel(IBatch *, const std::vector<std::string>&, int pal) const {
	return nullptr;
}

std::shared_ptr<IModel> MonospacedFont::buildModel(IBatch* batch, const std::vector<std::string>& text, int pal) const {
	size_t height = text.size();
	size_t width = text.front().size();
	for (size_t i = 0; i< text.size(); ++i) {
		width = std::max(width, text[i].size());
	}
	std::stringstream stream;
	stream << "[p=" << pal << "] ";
	for (const auto& row : text) {
		auto s32 = Font::getString32(row);
		int cw = 0;
		for (const auto &c: s32) {
			int index = _info.at(c);
			stream << index << " ";
			cw++;
		}
		if (cw < width) {
			for (size_t j = 0; j <width-cw; j++) {
				stream << "_ ";
			}
		}
		stream << "| ";
	}

	return std::make_shared<TileGrid>(batch, stream.str());
}

Font::Font(const YAML::Node &node, SpriteSheet *sheet) : _sheet(sheet) {
	_lineHeight = node["height"].as<float>();

}


MonospacedFont::MonospacedFont(const YAML::Node &node, SpriteSheet* sheet) : Font(node, sheet) {

	for (const auto& n : node["chars"]) {
		auto chars = n.first.as<std::string>();
		auto sss = getString32(chars);
		auto index = n.second.as<int>();
		char32_t w = sss[0];
		_info[w] = index;
	}
}

GenericFont::GenericFont(const YAML::Node &node, SpriteSheet* sheet) : Font(node, sheet)
{
	// need to get shader tex id from this
	auto tw = sheet->getSize().x;
	auto th = sheet->getSize().y;
	for (const auto& n : node["chars"]) {
		auto chars = n.first.as<std::string>();
		auto sss = getString32(chars);
		auto data = n.second.as<std::vector<float>>();
		int u = 0;
		for (char32_t w : sss) {
			if (u % 2 == 0) {
				m_info[w] = CharInfo{
						data[0] / tw, data[1] / th, data[2] / tw, data[3] / th,     // tex coords
						data[2], data[3],                                                    // size
						data[4],                                                            // advance
						data.size() > 5 ? data[5] : 0};                                         // offset y
			}
			u++;
		}
	}
}

std::u32string Font::getString32(const std::string& str) {
	return std::wstring_convert<std::codecvt_utf8<char32_t>, char32_t>().from_bytes(str.c_str());
}