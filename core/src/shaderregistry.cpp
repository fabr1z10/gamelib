#include "gamelib/shaderregistry.h"
#include "gamelib/shaders/basic_shaders.h"
#include <stdexcept>
#include "gamelib/primitives.h"
#include "gamelib/batch_shader.h"

ShaderRegistry::ShaderRegistry() {
	_shaderBuilders["sprite"] = [this] () {
		return std::make_shared<Shader<VertexTexture, QuadPrimitive>>(
				gamelib::shaders::sprite_vertex, gamelib::shaders::sprite_fragment);
	};

	_shaderBuilders["sprite_pal"] = [this] () {
		return std::make_shared<Shader<VertexTexturePalette, QuadPrimitive>>(
				gamelib::shaders::sprite_vertex_palette, gamelib::shaders::sprite_fragment_palette);
	};

}

IShader* ShaderRegistry::getShader(const std::string & id) {
	if (auto it = _shaders.find(id); it != _shaders.end()) {
		return it->second.get();
	}

	if (auto it = _shaderBuilders.find(id); it != _shaderBuilders.end()) {
		auto shader = it->second();
		_shaders[id] = shader;
		return shader.get();
	}
	throw std::runtime_error("Shader with id '" + id + "' not found");
}