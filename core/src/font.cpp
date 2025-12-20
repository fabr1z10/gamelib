#include "gamelib/font.h"
#include <locale>
#include <codecvt>


Font::Font(const YAML::Node &node, int width, int height)
{
	// need to get shader tex id from this
	auto tw = (float) width;
	auto th = (float) height;
	_lineHeight = node["height"].as<float>();
	for (const auto& n : node["chars"]) {
		auto chars = n.first.as<std::string>();
		auto sss = getString32(chars);
		auto data = n.second.as<std::vector<float>>();
		int u = 0;
		for (char32_t w : sss) {
			//std::cout << "loading character: " << w << "\n";
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