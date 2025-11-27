#include "gamelib/tex.h"
#include <png.h>
#include <stdexcept>
#include <iostream>
#include <vector>
#include <glm/glm.hpp>
#include <cstring>
#include "gamelib/file.h"

std::unordered_map<std::string, std::shared_ptr<Tex>> Tex::_textureStore;

Palette::Palette(const std::string &name) : _name(name) {}

void Palette::addColor(int index, glm::ivec4 color) {
	_colors[index] = color;
}

std::shared_ptr<Tex> Tex::getTexture(const std::string &filename) {
	auto it = Tex::_textureStore.find(filename);
	if (it == Tex::_textureStore.end()) {
		auto tex = std::make_shared<Tex>(filename);
		Tex::_textureStore[filename] = tex;
		return tex;
	} else {
		return it->second;
	}
}

Tex::Tex(const std::string &filename) : _texId(0), _paletteId(0) {
	// remember 0 means no texture bound!
	load(filename);
}

Tex::~Tex() {
	if (_texId)
		glDeleteTextures(1, &_texId);
	if (_paletteId)
		glDeleteTextures(1, &_paletteId);
}

void Tex::load(const std::string &filename, std::vector<Palette>* palettes)
{
	File file(filename);

	if (!file.good())
		throw std::runtime_error("Can't find file: " + filename);

	// --- PNG setup ---
	unsigned char sig[8];
	fread(sig, 1, 8, file.get());
	if (!png_check_sig(sig, 8))
		throw std::runtime_error(filename + ": not a PNG file");

	png_structp png = png_create_read_struct(PNG_LIBPNG_VER_STRING, nullptr, nullptr, nullptr);
	png_infop info = png_create_info_struct(png);
	if (!png || !info) throw std::runtime_error("libpng OOM");

	if (setjmp(png_jmpbuf(png))) {
		png_destroy_read_struct(&png, &info, nullptr);
		throw std::runtime_error("libpng error reading " + filename);
	}

	png_init_io(png, file.get());
	png_set_sig_bytes(png, 8);
	png_read_info(png, info);

	_width  = png_get_image_width(png, info);
	_height = png_get_image_height(png, info);
	int bit_depth  = png_get_bit_depth(png, info);
	int color_type = png_get_color_type(png, info);

	// Expand palette/gray → 8-bit but DO NOT convert palette → RGBA
	if (bit_depth < 8)
		png_set_packing(png);

	bool isIndexed = (color_type == PNG_COLOR_TYPE_PALETTE);

	if (!isIndexed) {
		// Convert all non-indexed formats → RGBA
		if (color_type == PNG_COLOR_TYPE_GRAY)
			png_set_gray_to_rgb(png);
		if (color_type == PNG_COLOR_TYPE_RGB)
			png_set_add_alpha(png, 0xFF, PNG_FILLER_AFTER);
		if (png_get_valid(png, info, PNG_INFO_tRNS))
			png_set_tRNS_to_alpha(png);
		_format = Format::RGBA;
	} else {
		_format = Format::INDEXED;
	}

	png_read_update_info(png, info);
	int rowbytes = png_get_rowbytes(png, info);

	// --- read pixels ---
	std::vector<unsigned char> pixels(_height * rowbytes);
	std::vector<png_bytep> rows(_height);
	for (int y = 0; y < _height; y++)
		rows[y] = &pixels[y * rowbytes];

	png_read_image(png, rows.data());

	// --- Indexed PNG case ---
	if (isIndexed)
	{
		// Read palette
		png_colorp plte = nullptr;
		png_bytep trans = nullptr;
		int num_palette = 0, num_trans = 0;

		png_get_PLTE(png, info, &plte, &num_palette);
		if (png_get_valid(png, info, PNG_INFO_tRNS))
			png_get_tRNS(png, info, &trans, &num_trans, nullptr);

		const int PALETTE_SIZE = 256 * 4;
		int num_palettes = 1 + (palettes ? palettes->size() : 0);

		std::vector<unsigned char> pal(num_palettes * PALETTE_SIZE, 0);

		// base palette
		for (int i = 0; i < num_palette; i++) {
			pal[i*4+0] = plte[i].red;
			pal[i*4+1] = plte[i].green;
			pal[i*4+2] = plte[i].blue;
			pal[i*4+3] = (i < num_trans) ? trans[i] : 255;
		}

		// additional palettes
		if (palettes) {
			int pIndex = 1;
			for (auto &P : *palettes) {
				std::memcpy(&pal[pIndex * PALETTE_SIZE], &pal[0], PALETTE_SIZE);

				for (auto &[idx, col] : P.getColors()) {
					int off = pIndex*PALETTE_SIZE + idx*4;
					pal[off+0] = col.r;
					pal[off+1] = col.g;
					pal[off+2] = col.b;
					pal[off+3] = col.a;
				}
				pIndex++;
			}
		}

		// upload index texture
		glGenTextures(1, &_texId);
		glBindTexture(GL_TEXTURE_2D, _texId);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_R8UI, _width, _height, 0,
					 GL_RED_INTEGER, GL_UNSIGNED_BYTE, pixels.data());
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

		// upload palette texture
		glGenTextures(1, &_paletteId);
		glBindTexture(GL_TEXTURE_2D, _paletteId);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8,
					 256, num_palettes,
					 0, GL_RGBA, GL_UNSIGNED_BYTE, pal.data());
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

		glBindTexture(GL_TEXTURE_2D, 0);
		png_destroy_read_struct(&png, &info, nullptr);
		return;
	}

	// --- RGBA PNG case ---
	glGenTextures(1, &_texId);
	glBindTexture(GL_TEXTURE_2D, _texId);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, _width, _height, 0,
				 GL_RGBA, GL_UNSIGNED_BYTE, pixels.data());
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glBindTexture(GL_TEXTURE_2D, 0);

	png_destroy_read_struct(&png, &info, nullptr);
}

