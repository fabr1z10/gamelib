#pragma once

#include <string_view>

namespace gamelib::shaders {

	inline const std::string_view sprite_vertex = R"(
	#version 430 core
	layout (location = 0) in vec3 aPosition;
	layout (location = 1) in vec2 aTexCoords;
	uniform mat4 pv_mat;
	out vec2 TexCoords;
	void main() {
		gl_Position = pv_mat * vec4(aPosition, 1);
		TexCoords = aTexCoords;
	}
	)";

	inline const std::string_view sprite_fragment = R"(
	#version 430 core
	in vec2 TexCoords;
	out vec4 FragColor;
	uniform sampler2D uTexture;
	void main() {
		FragColor = texture(uTexture, TexCoords);
	}
	)";




}