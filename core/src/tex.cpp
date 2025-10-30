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

void Tex::load(const std::string &filename, std::vector<Palette>* palettes) {
	File file(filename);

	if (!file.good()) {
		throw std::runtime_error("Can't find file: " + filename);
	};

	unsigned char sig[8];
	fread(sig, 1, 8, file.get());
	if (!png_check_sig(sig, 8)) {
		throw std::runtime_error("File " + filename + ": bad signature (not a PNG file).");
	}
	auto png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
	if (!png_ptr) {
		throw std::runtime_error("Out of memory opening " + filename);
	}
	auto info_ptr = png_create_info_struct(png_ptr);
	if (!info_ptr) {
		png_destroy_read_struct(&png_ptr, NULL, NULL);
		exit(1);   /* out of memory */
	}
	if (setjmp(png_jmpbuf(png_ptr))) {
		png_destroy_read_struct(&png_ptr, &info_ptr, NULL);
		exit(1);
	}
	png_init_io(png_ptr, file.get());
	png_set_sig_bytes(png_ptr, 8);  /* we already read the 8 signature bytes */

	png_read_info(png_ptr, info_ptr);  /* read all PNG info up to image data */
	_width = png_get_image_width(png_ptr, info_ptr);
	_height = png_get_image_height(png_ptr, info_ptr);

	auto bitDepth = (int) png_get_bit_depth(png_ptr, info_ptr);
	auto colorType = (int) png_get_color_type(png_ptr, info_ptr);
	std::cout << " -- width: " << _width << std::endl;
	std::cout << " -- height: " << _height << std::endl;
	std::cout << " -- bit depth: " << bitDepth << std::endl;
	std::cout << " -- color type: " << colorType << std::endl;
	if (colorType == PNG_COLOR_TYPE_PALETTE) {
		_format = Format::INDEXED;
		auto _row_pointers = new png_bytep[_height];
		unsigned int bytesPerRow = png_get_rowbytes(png_ptr, info_ptr);
		auto _pixels = new unsigned char[bytesPerRow * _height];
		auto data = new unsigned char[_width * _height];

		for (int i=0; i < _height; i++) {
			_row_pointers[i] = _pixels + i*bytesPerRow;
		}

		auto pixel_per_byte = 8 / bitDepth;
		unsigned char initSelector;
		switch (bitDepth) {
			case 1:
				initSelector = 0x80;
				break;
			case 2:
				initSelector = 0xC0;
				break;
			case 4:
				initSelector = 0xF0;
				break;
			case 8:
				initSelector = 0xFF;
				break;
		}
		png_read_image(png_ptr, _row_pointers);
		int k = 0;
		int pixel_per_row = 0;
		for (int i = 0; i < bytesPerRow * _height; ++i) {
			// read byte
			unsigned char selector = initSelector;
			// std::cout << " current byte: " << std::hex << (int) _pixels[i] << "\n";
			for (int j = 0; j < pixel_per_byte; ++j) {
				auto pixel = (_pixels[i] & selector) >> (8 - bitDepth * (j + 1));
				data[k++] = pixel;
				// std::cout << "added pix: " << (int) pixel << "\n";
				pixel_per_row++;
				if (pixel_per_row >= _width) {
					pixel_per_row = 0;
					break;
				}
				selector >>= bitDepth;
			}
		}

		// now read palette
		png_colorp palette = nullptr;
		png_bytep trans_alpha = nullptr;
		int num_palette = 0;

		int num_trans = 0;
		if (png_get_PLTE(png_ptr, info_ptr, &palette, &num_palette) != PNG_INFO_PLTE) {
			throw std::runtime_error("File " + filename + ": Failed to get PNG palette.");
		}
		// Get alpha values (optional)
		if (png_get_valid(png_ptr, info_ptr, PNG_INFO_tRNS)) {
			png_get_tRNS(png_ptr, info_ptr, &trans_alpha, &num_trans, nullptr);
		}
		// Allocate RGBA palette
		const int PALETTE_SIZE = 256 * 4;
		int num_palettes = 1 + (palettes ? (int) palettes->size() : 0);
		auto pal = new unsigned char[num_palettes * PALETTE_SIZE];
		std::memset(pal, 0, num_palettes * PALETTE_SIZE);  // This sets all bytes in pal to 0

		// map color to index
		std::unordered_map<int, glm::ivec4> paletteMap;
		// Fill RGBA values
		for (int i = 0; i < num_palette; ++i) {
			pal[i * 4 + 0] = palette[i].red;
			pal[i * 4 + 1] = palette[i].green;
			pal[i * 4 + 2] = palette[i].blue;
			unsigned char alpha;
			if (i < num_trans) {
				alpha = trans_alpha[i];  // transparency from tRNS
			} else {
				alpha = 255;  // fully opaque
			}
			pal[i * 4 + 3] = alpha;
			paletteMap[i] = glm::ivec4(palette[i].red, palette[i].green, palette[i].blue, alpha);
		}

		// now store other palettes! colors that are not changed stay the same
		// First, copy the palette to other palettes
		for (int i = 1; i < num_palettes; ++i) {
			std::memcpy(&pal[i * PALETTE_SIZE], &pal[0], PALETTE_SIZE);
		}

		// now replace colors according to input
		int palIndex = 1;
		for (const auto& pals : *palettes) {
			int offset = palIndex * PALETTE_SIZE;
			for (const auto& [key, value] : pals.getColors()) {
				int index = offset + 4 * key;
				pal[index] = value.r;
				pal[index + 1] = value.g;
				pal[index + 2] = value.b;
				pal[index + 3] = value.a;
			}
			palIndex++;
		}
		for (int i = 0; i < 1024; ++i) {
			std::cout << i << " " <<(int) pal[i] << "\n";
		}
		glGenTextures (1, &_texId);
		glBindTexture (GL_TEXTURE_2D, _texId);
		glTexImage2D (GL_TEXTURE_2D, 0, GL_R8UI , _width, _height, 0,  GL_RED_INTEGER, GL_UNSIGNED_BYTE, data);
		glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameterf (GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameterf (GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

		// generate the pal tex
		glGenTextures(1, &_paletteId);
		glBindTexture(GL_TEXTURE_2D, _paletteId);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, 256, num_palettes, 0, GL_RGBA, GL_UNSIGNED_BYTE, &pal[0]);
		glTexParameterf (GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameterf (GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);

		glBindTexture(GL_TEXTURE_2D, 0);
		//f//or (int i = 0; i < _height; ++i) {
		//	for (int j = 0; j < _width; ++j) {
		//		std::cout << "Pixel index at (" << j << ", " << i << "): " << (int)data[i * _width + j] << std::endl;
		//	}
		//}
	}
	return;
	exit(1);



	png_structp png = png_create_read_struct(PNG_LIBPNG_VER_STRING, nullptr, nullptr, nullptr);
	if (!png) {
		return;
	}

	png_infop info = png_create_info_struct(png);
	if (!info) {
		png_destroy_read_struct(&png, nullptr, nullptr);
		return;
	}

	if (setjmp(png_jmpbuf(png))) {
		png_destroy_read_struct(&png, &info, nullptr);
		return;
	}

	png_init_io(png, file.get());
	png_set_sig_bytes(png, 8);  /* we already read the 8 signature bytes */
	png_read_info(png, info);  /* read all PNG info up to image data */
	_width = png_get_image_width(png, info);
	_height = png_get_image_height(png, info);
	auto color_type =(int) png_get_color_type(png, info);
	auto bit_depth = (int)png_get_bit_depth(png, info);

	std::cout << " -- width: " << _width << std::endl;
	std::cout << " -- height: " << _height << std::endl;
	std::cout << " -- bit depth: " << bit_depth << std::endl;
	std::cout << " -- color type: " << color_type << std::endl;


	// Expand low-bit grayscale and palette images to 8 bits
	if (bit_depth < 8)
		png_set_expand_gray_1_2_4_to_8(png);
	if (png_get_valid(png, info, PNG_INFO_tRNS))
		png_set_tRNS_to_alpha(png);

	// Convert to RGBA if needed
	if (color_type == PNG_COLOR_TYPE_PALETTE) {
		// keep palette — don’t convert to RGBA
		_format = Format::INDEXED;
	} else if (color_type == PNG_COLOR_TYPE_RGB ||
			   color_type == PNG_COLOR_TYPE_RGB_ALPHA ||
			   color_type == PNG_COLOR_TYPE_GRAY ||
			   color_type == PNG_COLOR_TYPE_GRAY_ALPHA) {
		if (color_type == PNG_COLOR_TYPE_GRAY)
			png_set_gray_to_rgb(png);
		if (color_type == PNG_COLOR_TYPE_RGB)
			png_set_add_alpha(png, 0xFF, PNG_FILLER_AFTER);
		_format = Format::RGBA;
	} else {
		png_destroy_read_struct(&png, &info, nullptr);
		throw std::runtime_error("Unsupported PNG color type");
	}
	png_read_update_info(png, info);
	if (_format == Format::INDEXED) {
		// Allocate RGBA palette
		const int PALETTE_SIZE = 256 * 4;			// each palette is 256 colors, 4 byte per color
		int num_palettes = 1;
		auto pal = new unsigned char[1 * PALETTE_SIZE];
		png_colorp palette = nullptr;
		png_bytep trans_alpha = nullptr;
		int num_palette = 0;
		int num_trans = 0;
		png_get_PLTE(png, info, &palette, &num_palette);
		std::cout << "Palette entries: " << num_palette << std::endl;
		// Get alpha values (optional)
		if (png_get_valid(png, info, PNG_INFO_tRNS)) {
			png_get_tRNS(png, info, &trans_alpha, &num_trans, nullptr);
		}
		std::memset(pal, 0, num_palettes * PALETTE_SIZE);  // This sets all bytes in pal to 0
		for (int i = 0; i < num_palette; ++i) {
			pal[i * 4 + 0] = palette[i].red;
			pal[i * 4 + 1] = palette[i].green;
			pal[i * 4 + 2] = palette[i].blue;
			unsigned char alpha;
			if (i < num_trans) {
				alpha = trans_alpha[i];  // transparency from tRNS
			} else {
				alpha = 255;  // fully opaque
			}
			pal[i * 4 + 3] = alpha;
			// now map hex to index
			//paletteMap[packRGBA(palette[i].red, palette[i].green, palette[i].blue, alpha)] = i;
		}

		// Read pixel indices
		std::vector<uint8_t> indices(_width * _height);
		std::vector<png_bytep> rows(_height);
		for (int y = 0; y < _height; ++y)
			rows[y] = &indices[y * _width];

		png_read_image(png, rows.data());
		for (size_t i = 0; i < _height; i++) {
			for (size_t j = 0; j < _width; j++) {
				std::cout << "Pixel index at (" << j << ", " << i << "): " << (int)indices[i * _width + j] << std::endl;
			}
		}
		exit(1);

		// upload index texture (GL_R8)
		glGenTextures(1, &_texId);
		glBindTexture(GL_TEXTURE_2D, _texId);
		//glTexImage2D(GL_TEXTURE_2D, 0, GL_R8, _width, _height, 0, GL_RED, GL_UNSIGNED_BYTE, indices.data());
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glBindTexture(GL_TEXTURE_2D, 0);

		// upload palette (GL_RGBA8)
//		std::vector<glm::u8vec4> paletteData(num_palette);
//		for (int i = 0; i < num_palette; ++i) {
//			paletteData[i] = glm::u8vec4(palette[i].red, palette[i].green, palette[i].blue, 255);
//			std::cout << "Palette color " << i << ": ("
//					  << (int) palette[i].red << ", "
//					  << (int) palette[i].green << ", "
//					  << (int) palette[i].blue << ")" << std::endl;
//		}
		glGenTextures(1, &_paletteId);
		glBindTexture(GL_TEXTURE_2D, _paletteId);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, 256, num_palettes, 0, GL_RGBA, GL_UNSIGNED_BYTE, &pal[0]);
		glTexParameterf (GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameterf (GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);

		glBindTexture(GL_TEXTURE_2D, 0);


	} else {
		std::vector<uint8_t> pixels(_width * _height * 4);
		std::vector<png_bytep> rows(_height);
		for (int y = 0; y < _height; ++y)
			rows[y] = &pixels[y * _width * 4];
		png_read_image(png, rows.data());

		glGenTextures(1, &_texId);
		glBindTexture(GL_TEXTURE_2D, _texId);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, _width, _height, 0, GL_RGBA, GL_UNSIGNED_BYTE, pixels.data());
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glBindTexture(GL_TEXTURE_2D, 0);
	}

	png_destroy_read_struct(&png, &info, nullptr);

}

