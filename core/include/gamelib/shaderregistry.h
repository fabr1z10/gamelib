#pragma once

#include "gamelib/shader.h"

class ShaderRegistry {
public:
	ShaderRegistry();

	IShader* getShader(const std::string&);

private:
	std::unordered_map<std::string, std::shared_ptr<IShader>> _shaders;
	std::unordered_map<std::string, std::function<std::shared_ptr<IShader>()>> _shaderBuilders;

};