#pragma once

#include <glm/glm.hpp>
#include <unordered_map>
#include <string>
#include <memory>
#include "gamelib/batch.h"

class Node;


class Room : public std::enable_shared_from_this<Room>{
public:
	Room();

	void update(double) ;

	void initialize();

	std::shared_ptr<Node> getRootNode();

	void draw();

	bool isComplete() const;

	/*
	 * Set clear color passing r,g,b values as integers in range 0-255
	 */
	void setClearColor(glm::ivec3);

	IBatch* getBatch(const std::string&);

	Camera* getCamera(const std::string& key);

	void addBatch(const std::string& key, std::shared_ptr<IBatch> batch);

	void addCamera(const std::string& key, std::shared_ptr<Camera> camera);
private:
	glm::vec3 _clearColor;
	bool _complete = false;
	std::unordered_map<std::string, std::shared_ptr<IBatch>> _batches;
	std::unordered_map<std::string, std::shared_ptr<Camera>> _cameras;
	std::shared_ptr<Node> _rootNode;
	std::shared_ptr<IShader> _blitShader;
	unsigned int _quadVAO, _quadVBO;
	GLuint _fb, _color, _depth;
};

inline bool Room::isComplete() const {
	return _complete;
}

inline std::shared_ptr<Node> Room::getRootNode() {
	return _rootNode;
}

