#pragma once

#include <vector>
#include <memory>
#include <glm/glm.hpp>
#include "gamelib/model.h"


class Node {

public:
	Node();

	void add(std::shared_ptr<Node> child);

	void update(double dt);

	void remove();

	void draw();

	glm::vec3 getWorldPosition() const;

	const glm::mat4& getWorldMatrix() const;

	void setPosition(glm::vec3);

	void setModel(std::shared_ptr<IModel> model);
protected:
	bool _toBeRemoved;
	void notifyMove();
	std::vector<std::shared_ptr<Node>> _children;
	std::shared_ptr<IModel> _model;
	Node* _parent;
	glm::mat4 _modelMatrix;
	glm::mat4 _worldMatrix;
};

