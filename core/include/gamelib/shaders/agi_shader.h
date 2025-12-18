#pragma once

#include <string_view>

namespace gamelib::shaders {

	inline const std::string_view agi_vertex = R"(
	#version 430 core
	layout (location = 0) in vec3 aPosition;
	layout (location = 1) in vec2 aTexCoords;
	layout (location = 2) in int vTexIndex;
	uniform mat4 mvp_mat;
	out vec2 TexCoords;
	out flat int TexIndex;
	void main() {
		gl_Position = mvp_mat * vec4(aPosition, 1);
		TexCoords = aTexCoords;
        TexIndex = vTexIndex;
	}
	)";

	inline const std::string_view agi_fragment = R"(
	#version 430 core

	uniform usampler2D uTexture;
	uniform sampler2D texture_palette;
	uniform sampler2D uPriorityTex;

	in vec2 TexCoords;
	in flat int TexIndex;

	out vec4 FragColor;

	void main()
	{
		uint index = texture(uTexture, TexCoords).r;
		vec4 texColor = texelFetch(texture_palette, ivec2(int(index), TexIndex), 0);
	    FragColor = texColor;
	    float depth = texture(uPriorityTex, TexCoords).r;    // get z
	    gl_FragDepth = 1.0 - depth;
	}
	)";


}