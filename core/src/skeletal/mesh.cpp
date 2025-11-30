#include "gamelib/skeletal/mesh.h"
#include "gamelib/shader.h"
#include "gamelib/primitives.h"
#include "gamelib/tex.h"
#include <filesystem>


void Mesh::generateKeyPoints(const YAML::Node &info) {
	if (info["keypoints"]) {
		auto kpNode = info["keypoints"];
		for (YAML::const_iterator it = kpNode.begin(); it != kpNode.end(); ++it) {
			std::string name = it->first.as<std::string>();
			auto point = it->second.as<glm::vec2>();
			_keyPoints[name] = point;
		}
	}
}
Mesh::Mesh(const std::string& file, const std::string& id) {
	namespace fs = std::filesystem;

	try {
		fs::path p(file);
		YAML::Node data = YAML::LoadFile(p.string());
		YAML::Node info = data[id];
		auto points = info["vertices"].as<std::vector<float>>();
		auto indices = info["indices"].as<std::vector<unsigned>>();
		auto texture = info["texture"].as<std::string>();
		auto tex = Tex::getTexture(p.parent_path() / texture);
		_texId = tex->getId();
		generateBuffers(points, indices);
		generateKeyPoints(info);

	} catch (const YAML::BadFile& e) {
		throw std::runtime_error(std::string("Could not open config file: ") + e.what());
	} catch (const YAML::Exception &e) {
		throw std::runtime_error(std::string("YAML parsing error: ") + e.what());
	} catch (const std::exception &e) {
		throw std::runtime_error(std::string("Error loading config: ") + e.what());
	}


}

void Mesh::generateBuffers(const std::vector<float>& vertices, const std::vector<unsigned>& indices) {
    glGenVertexArrays(1, &_vao);
    glBindVertexArray(_vao);

    glGenBuffers(1, &_vbo);
    glBindBuffer(GL_ARRAY_BUFFER, _vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * vertices.size(), &vertices[0], GL_STATIC_DRAW);

	VertexSkeletal::setupVertices();

    glGenBuffers(1, &_ebo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * indices.size(), &indices[0], GL_STATIC_DRAW);
    glBindVertexArray(0);
    _size = indices.size();
}

void Mesh::draw(IShader* s) {
    if (_texId != GL_INVALID_VALUE) {
        s->setInt("texture_diffuse1", 0);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, _texId);
    }
    glBindVertexArray(_vao);
    glDrawElements(GL_TRIANGLES, _size, GL_UNSIGNED_INT, (GLvoid*)(sizeof(GLuint) * 0));
    glBindVertexArray(0);
}
