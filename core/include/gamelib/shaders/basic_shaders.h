#pragma once

#include <string_view>

namespace gamelib::shaders {

	inline const std::string_view color_vertex = R"(
	#version 430 core

	layout (location = 0) in vec3 vPosition;
	layout (location = 1) in vec4 vColor;

	uniform mat4 mvp_mat;

	out vec4 Color;

	void main() {
	    gl_Position = mvp_mat * vec4(vPosition, 1);
        Color = vColor;
    }
	)";

	inline const std::string_view color_fragment = R"(
	#version 430 core

	in vec4 Color;
	out vec4 FragColor;

	void main()
	{
		if (Color.a < 0.5) {
			discard;
		}
		FragColor = Color;
	}

	)";

	inline const std::string_view color_normal_vertex = R"(
	#version 430 core

	layout (location = 0) in vec3 vPosition;
	layout (location = 1) in vec4 vColor;
	layout (location = 2) in vec3 vNormal;

	uniform mat4 mvp_mat;
	uniform mat3 model_mat;

	out vec4 Color;
	out vec3 Normal;

	void main() {
	    gl_Position = mvp_mat * vec4(vPosition, 1);
        Color = vColor;
		Normal = model_mat * vNormal;
    }
	)";

	inline const std::string_view color_normal_fragment = R"(
	#version 430 core

	in vec4 Color;
	in vec3 Normal;

	out vec4 FragColor;

	uniform vec3 lightDir;   // must be normalized

	void main()
	{
		if (Color.a < 0.5) {
			discard;
		}
    	vec3 N = normalize(Normal);
    	float diff = max(dot(N, -lightDir), 0.0);   // -lightDir = direction light travels
		FragColor = vec4(Color.rgb * diff, Color.a);
	}

	)";


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
	uniform mat4 mvp_mat;
	out vec2 TexCoords;
	void main() {
		gl_Position = mvp_mat * vec4(aPosition, 1);
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
	uniform mat4 mvp_mat;
	out vec2 TexCoords;
	out flat int TexIndex;
	void main() {
		gl_Position = mvp_mat * vec4(aPosition, 1);
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