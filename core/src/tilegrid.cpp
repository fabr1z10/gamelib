#include "gamelib/tilegrid.h"
#include <istream>
#include <sstream>


TileGrid::TileGrid(IBatch *batch, const std::string& desc) : Model<VertexTexturePalette, QuadPrimitive>(batch),
	_description(desc) {
	parseDescription();
	this->allocate(_width * _height);
}

void TileGrid::parseDescription() {
	_tiles.clear();
	// reset size
	_width = 0;
	_height = 0;

	std::vector<std::vector<Tile>> rows;
	std::vector<Tile> currentRow;

	std::istringstream ss(_description);
	std::string token;

	while (ss >> token) {
		if (token == "|") {
			if (currentRow.empty()) {
				throw std::runtime_error("Empty row in tile grid.");
			}

			rows.push_back(currentRow);
			if (_width == 0) {
				_width = currentRow.size();
			} else if (currentRow.size() != _width) {
				throw std::runtime_error("Non-rectangular tile grid.");
			}
			currentRow.clear();
		} else {
			Tile t;
			if (token == EMPTY_TOKEN) {
				t.index = EMPTY_INDEX;
			} else {
				auto pos = token.find('(');
				if (pos == std::string::npos) {
					// no args
					t.index = static_cast<uint16_t>(std::stoi(token));
				} else {
					t.index = static_cast<uint16_t>(std::stoi(token.substr(0, pos)));
					auto end = token.find(')');
					std::string props = token.substr(pos+1, end-pos-1);
					std::istringstream ps(props);
					std::string p;
					while (std::getline(ps, p, ',')) {
						if (p.empty()) continue;
						if (p[0]=='p') {
							auto eq = p.find('=');
							t.palette = std::stoi(p.substr(eq+1));
						} else if (p=="h") t.flipX = true;
						else if (p=="v") t.flipY = true;
					}

				}
			}
			currentRow.push_back(t);
		}
	}
	// last row
	if (!currentRow.empty()) {
		rows.push_back(currentRow);
		if (_width == 0)
			_width = (int)currentRow.size();
		else if ((int)currentRow.size() != _width)
			throw std::runtime_error("Non-rectangular tile grid");
	}

	_height = (int)rows.size();
	if (_width == 0 || _height == 0)
		throw std::runtime_error("Empty tile grid");

	// Flatten rows (row-major)
	for (auto& row : rows) {
		for (auto& tile : row)
			_tiles.push_back(tile);
	}
}

void TileGrid::refresh() {
	auto pos = this->getWorldPosition();
	auto ts = _batch->getSpriteSheet()->getTileSize();
	glm::vec3 curr;

	for (int y = 0; y < _height; ++y) {
		float cy = - y * ts;
		for (int x = 0; x < _width; ++x) {
			int quadIndex = y * _width + x;
			float cx = x * ts;
			const Tile& tile = _tiles[quadIndex];
			VertexTexturePalette* p = this->_vertices[quadIndex];
			if (tile.index == -1) {
				continue;
			}
			auto texCoords = _batch->getSpriteSheet()->getTexCoords(tile.index);
			curr = pos + glm::vec3(cx, cy, 0);

			// compute flipped tex coords
			float tx0 = texCoords.x;
			float tx1 = texCoords.x + texCoords.z;
			float ty0 = texCoords.y;
			float ty1 = texCoords.y + texCoords.w;

			if (tile.flipX) std::swap(tx0, tx1);
			if (tile.flipY) std::swap(ty0, ty1);

			// start with TOP left
			p->position = curr;
			p->texCoord = {tx0, ty0}; //glm::vec2(texCoords.x, texCoords.y);
			p->paletteId = tile.palette;

			(p+1)->position = curr + glm::vec3(0, -ts, 0);
			(p+1)->texCoord = {tx0, ty1};// glm::vec2(texCoords.x, texCoords.y + texCoords.w);
			(p+1)->paletteId = tile.palette;

			(p+2)->position = curr + glm::vec3(ts, -ts, 0);
			(p+2)->texCoord = {tx1, ty1}; //glm::vec2(texCoords.x + texCoords.z, texCoords.y + texCoords.w);
			(p+2)->paletteId = tile.palette;

			(p+3)->position = curr + glm::vec3(ts, 0, 0);
			(p+3)->texCoord = {tx1, ty0}; //glm::vec2(texCoords.x + texCoords.z, texCoords.y);
			(p+3)->paletteId = tile.palette;

		}
	}

}



