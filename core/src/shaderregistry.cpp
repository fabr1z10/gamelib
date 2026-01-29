#include "gamelib/shaderregistry.h"
#include "gamelib/shaders/basic_shaders.h"
#include "gamelib/shaders/skeletal_shader.h"
#include "gamelib/shaders/agi_shader.h"
#include <stdexcept>
#include "gamelib/primitives.h"
#include "gamelib/batch_shader.h"
#include "gamelib/agi/agibatch.h"

ShaderRegistry::ShaderRegistry() {
//	_shaderBuilders["sprite"] = [this] () {
//		return std::make_shared<Shader<VertexTexture, QuadPrimitive>>(
//				gamelib::shaders::sprite_vertex, gamelib::shaders::sprite_fragment);
//	};

	_shaderBuilders["sprite_pal"] = [this] () {
		return std::make_shared<Shader<
		        VertexTexturePalette,
				QuadPrimitive,
				DynamicBatch<VertexTexturePalette, QuadPrimitive>>>(
			gamelib::shaders::sprite_vertex_palette, gamelib::shaders::sprite_fragment_palette);
	};


//
//	_shaderBuilders["triangle_color"] = [this] () {
//		return std::make_shared<Shader<VertexColorNormal, TrianglePrimitive, DynamicBatch<VertexColorNormal, TrianglePrimitive>>>(
//			gamelib::shaders::color_normal_vertex, gamelib::shaders::color_normal_fragment);
//	};
//
	_shaderBuilders["quad_texture_repeat"] = [this] () {
		return std::make_shared<Shader<VertexTextureRepeat, QuadPrimitive, DynamicBatch<VertexTextureRepeat, QuadPrimitive>>>(
				gamelib::shaders::texture_normal_vertex, gamelib::shaders::texture_normal_fragment);
	};

	_shaderBuilders["line_color"] = [this] () {
		return std::make_shared<Shader<VertexColor, LinePrimitive, DynamicBatch<VertexColor, LinePrimitive>>>(
				gamelib::shaders::color_vertex, gamelib::shaders::color_fragment);
	};

	_shaderBuilders["triangle_color"] = [this] () {
		return std::make_shared<Shader<VertexColor, TrianglePrimitive, DynamicBatch<VertexColor, TrianglePrimitive>>>(
				gamelib::shaders::color_vertex, gamelib::shaders::color_fragment);
	};
//
//	_shaderBuilders["skeletal"] = [this] () {
//		return std::make_shared<Shader<VertexSkeletal, TrianglePrimitive>>(
//				gamelib::shaders::skeletal_vertex, gamelib::shaders::skeletal_fragment);
//	};
//
	_shaderBuilders["agi"] = [this] () {
		return std::make_shared<Shader<VertexTexturePalette, QuadPrimitive, AGIBatch<VertexTexturePalette, QuadPrimitive>>>(
				gamelib::shaders::agi_vertex, gamelib::shaders::agi_fragment);
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