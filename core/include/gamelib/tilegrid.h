#pragma once

#include "gamelib/model.h"

struct Tile {
	uint16_t index;
	uint8_t palette = 0;
	bool flipX = false;
	bool flipY = false;
};


class TileGrid : public Model<VertexTexturePalette, QuadPrimitive> {
public:
	static constexpr const char* EMPTY_TOKEN = "_";
	static constexpr int EMPTY_INDEX = -1;

	TileGrid(IBatch* batch, const std::string& desc);

	virtual void setVertex(VertexTexturePalette* v, const glm::vec3& pos, const glm::vec2& uv) {
		v->position = pos;
		v->texCoord = uv;
	}

	void refresh() override;

private:
	void parseDescription();


	std::vector<Tile> _tiles;
	std::string _description;
	int _width;
	int _height;
};