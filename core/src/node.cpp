#include "gamelib/node.h"
#include <algorithm>

Node::Node() : _parent(nullptr), _modelMatrix(1.0f), _worldMatrix(1.0f), _toBeRemoved(false) {
}

void Node::remove() {
	_toBeRemoved = true;
}

void Node::add(std::shared_ptr<Node> child) {
	child->_parent = this;
	_children.push_back(child);
}

glm::vec3 Node::getWorldPosition() const {
	return glm::vec3(_worldMatrix[3]);
}

const glm::mat4 &Node::getWorldMatrix() const {
	return _worldMatrix;
}

void Node::setPosition(glm::vec3 pos) {
	_modelMatrix[3] = glm::vec4(pos, 1.f);
	notifyMove();
}

void Node::setModel(std::shared_ptr<IModel> model) {
	_model = model;
	_model->setOwner(this);
	_model->refresh();
}

void Node::update(double dt) {
	auto shouldRemove = [] (const std::shared_ptr<Node>& node) {
		return node->_toBeRemoved;
	};
	// first remove all nodes marked as removal
	_children.erase(std::remove_if(_children.begin(), _children.end(), shouldRemove), _children.end());

	if (_model) _model->update();

	for (auto& child : _children) {
		child->update(dt);
	}
	// TODO other stuff
}

void Node::draw() {
	// TODO
}

void Node::notifyMove() {
	if (_parent != nullptr) {
		_worldMatrix = _parent->getWorldMatrix() * _modelMatrix;
	} else {
		_worldMatrix = _modelMatrix;
	}
	for (auto& child : _children) {
		child->notifyMove();
	}
	if (_model) {
		_model->refresh();
	}
}