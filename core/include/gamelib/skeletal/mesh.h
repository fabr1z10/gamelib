#pragma once

#include <vector>
#include <string>
#include <GL/glew.h>
#include "gamelib/yaml_extension.h"

class IShader;

class Mesh {
public:
    Mesh(const std::string& file, const std::string& id);
    void draw(IShader*);
private:
    void generateBuffers(const std::vector<float>& points, const std::vector<unsigned>& indices);
	void generateKeyPoints(const YAML::Node& info);
    GLuint _size;
    GLuint _vao;
    GLuint _vbo;
    GLuint _ebo;
    GLuint _texId;
	std::unordered_map<std::string, glm::vec2> _keyPoints;
};