#pragma once

#include <glm/glm.hpp>
#include <unordered_map>
#include <unordered_set>
#include <string>
#include <memory>
#include "gamelib/batch.h"

class Node;
class ICollisionEngine;

class Room {
public:
	Room();

	void update(double) ;

	virtual void initialize();

	std::shared_ptr<Node> getRootNode();

	void draw();

	void cleanup();
	bool isComplete() const;

	/*
	 * Set clear color passing r,g,b values as integers in range 0-255
	 */
	void setClearColor(glm::ivec3);

	IBatch* getBatch(const std::string&);

	Camera* getCamera(const std::string& key);

	void addBatch(const std::string& key, std::shared_ptr<IBatch> batch);

	void addShader(IShader* shader);

	void addCamera(const std::string& key, std::shared_ptr<Camera> camera);

	ICollisionEngine* getCollisionEngine() const;

	void addCollisionEngine(std::shared_ptr<ICollisionEngine>);
private:
	glm::vec3 _clearColor;
	bool _complete = false;
	std::unordered_map<std::string, std::shared_ptr<IBatch>> _batches;
	std::unordered_map<std::string, std::shared_ptr<Camera>> _cameras;
	std::shared_ptr<Node> _rootNode;
	std::shared_ptr<IShader> _blitShader;
	unsigned int _quadVAO, _quadVBO;
	std::unordered_set<IShader*> _shadersForStaticRendering;
	GLuint _fb, _color, _depth;
	std::shared_ptr<ICollisionEngine> _collisionEngine;
};

inline bool Room::isComplete() const {
	return _complete;
}

inline std::shared_ptr<Node> Room::getRootNode() {
	return _rootNode;
}

inline ICollisionEngine *Room::getCollisionEngine() const {
	return _collisionEngine.get();
}

