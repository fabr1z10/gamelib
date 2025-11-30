#include "gamelib/spritesheet.h"
#include "gamelib/yaml_extension.h"
#include "gamelib/game.h"
#include "gamelib/shader.h"
#include "gamelib/model.h"

std::unordered_map<std::string, std::shared_ptr<SpriteSheet>> SpriteSheet::_sheets;

std::shared_ptr<IModel> SpriteSheet::getModel(IBatch* batch, const std::string &id) {
	// check if this model is a QUAD
	auto it = _quads.find(id);
	std::shared_ptr<IModel> m;
	if (it != _quads.end()) {
		if (isIndexed()) {
			m = std::make_shared<QuadModelPal>(batch, it->second);
		} else {
			m = std::make_shared<QuadModel>(batch, it->second);
		}
	} else {
		auto it = _sprites.find(id);
		if (it != _sprites.end()) {
			if (isIndexed()) {
				m = std::make_shared<SpriteModelPal>(batch, it->second);
			} else {
				m = std::make_shared<SpriteModel>(batch, it->second);
			}
		}
	}
	return m;
}

std::shared_ptr<SpriteSheet> SpriteSheet::getSpriteSheet(const std::string &filename) {
	auto it = SpriteSheet::_sheets.find(filename);
	if (it == SpriteSheet::_sheets.end()) {
		auto sheet = std::make_shared<SpriteSheet>(filename);
		SpriteSheet::_sheets[filename] = sheet;
		return sheet;
	} else {
		return it->second;
	}
}

bool SpriteSheet::isIndexed() const {
	return _tex->getFormat() == Tex::Format::INDEXED;
}

SpriteSheet::SpriteSheet(const std::string &filename) {
	try {


		std::string fullName = Game::instance().getHomeDir() + "/" + filename;
		YAML::Node config = YAML::LoadFile(fullName);
		auto image = YAML::read<std::string>(config, "image");
		//_tex = Tex::getTexture(image);
		if (config["palettes"]) {
			std::cout << "Loading palettes for spritesheet " << filename << std::endl;
			for (auto p : config["palettes"]) {
				auto id = p.first.as<std::string>();
				Palette pal(id);
				for (auto col : p.second) {
					pal.addColor(col.first.as<int>(), col.second.as<glm::ivec4>());
				}
				_palettes.push_back(pal);
				_paletteNameToIndex[id] = _palettes.size();
			}
		}
		_tex = std::make_shared<Tex>();
		_tex->load(image, &_palettes);
		auto readQuad = [&] (const YAML::Node& node) -> QuadInfo {
			auto quad = node.as<QuadInfo>();
			quad.tx0 = static_cast<float>(quad.x) / _tex->getSize().x;
			quad.tx1 = static_cast<float>(quad.x + quad.width) / _tex->getSize().x;
			quad.ty0 = static_cast<float>(quad.y) / _tex->getSize().y;
			quad.ty1 = static_cast<float>(quad.y + quad.height) / _tex->getSize().y;
			return quad;
		};
		if (config["quads"]) {
			for (auto q : config["quads"]) {
				auto id = q.first.as<std::string>();
				_quads[id] = readQuad(q.second);
			}
		}
		if (config["sprites"]) {
			auto defaultTicks = Game::instance().getConfig()->getDefaultTicks();
			for (auto s : config["sprites"]) {
				SpriteInfo info;
				auto id = s.first.as<std::string>();
				for (auto a : s.second["animations"]) {
					auto animId = a.first.as<std::string>();
					Animation anim;
					anim.loopIndex = YAML::read<int>(a.second, "loop_index", 0);
					for (auto f : a.second["frames"]) {
						Frame frame;
						frame.quad = readQuad(f["quad"]);
						frame.ticks = YAML::read(f, "ticks", defaultTicks);
						anim.frames.push_back(frame);
					}
					info.addAnimation(animId, anim);
				}
				_sprites[id] = info;
			}
		}


	} catch (const YAML::BadFile& e) {
		throw std::runtime_error(std::string("Could not open config file: ") + e.what());
	} catch (const YAML::Exception &e) {
		throw std::runtime_error(std::string("YAML parsing error: ") + e.what());
	} catch (const std::exception &e) {
		throw std::runtime_error(std::string("Error loading config: ") + e.what());
	}
}


void SpriteSheet::setupGL(IShader* shader) const {
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, _tex->getId());
	GLint locDiffuse = glGetUniformLocation(shader->getProgramId(), "uTexture");
	glUniform1i(locDiffuse, 0);

	// Activate and bind texture_palette to texture unit 1
	if (isIndexed()) {
		auto palId = _tex->getPaletteId();
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, palId);
		GLint locPalette = glGetUniformLocation(shader->getProgramId(), "texture_palette");
		glUniform1i(locPalette, 1); // Bind to texture unit 1
	}
}