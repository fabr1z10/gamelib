#pragma once

#include <string_view>

namespace gamelib::shaders {

	inline const std::string_view blit_vertex = R"(
	#version 430 core
	layout (location = 0) in vec2 aPos;
	layout (location = 1) in vec2 aTexCoords;

	out vec2 TexCoords;

	void main()
	{
    	gl_Position = vec4(aPos.x, aPos.y, 0.0, 1.0);
    	TexCoords = aTexCoords;
	}
	)";

	inline const std::string_view blit_fragment = R"(
	#version 430 core
	out vec4 FragColor;

	in vec2 TexCoords;

	uniform sampler2D screenTexture;

	void main()
	{
	    FragColor = texture(screenTexture, TexCoords);
	}
	)";


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

	inline const std::string_view sprite_vertex_palette = R"(
	#version 430 core
	layout (location = 0) in vec3 aPosition;
	layout (location = 1) in vec2 aTexCoords;
	layout (location = 2) in int vTexIndex;
	uniform mat4 pv_mat;
	out vec2 TexCoords;
	out flat int TexIndex;
	void main() {
		gl_Position = pv_mat * vec4(aPosition, 1);
		TexCoords = aTexCoords;
        TexIndex = vTexIndex;
	}
	)";


	inline const std::string_view sprite_fragment_palette = R"(
	#version 430 core

	uniform usampler2D uTexture;
	uniform sampler2D texture_palette;  // where palette is stored as RGBA8

	in vec2 TexCoords;
	in flat int TexIndex;

	out vec4 FragColor;

	void main()
	{
		uint index = texture(uTexture, TexCoords).r;

		vec4 texColor = texelFetch(texture_palette, ivec2(int(index), TexIndex), 0);
 		if (texColor.a < 0.5) {
     		discard;
    	}
    	FragColor = texColor;
	}
	)";



}