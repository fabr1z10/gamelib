#include "gamelib/node.h"
#include <algorithm>
#include "gamelib/model.h"
#include <glm/gtx/transform.hpp>

Node::Node() : _parent(nullptr), _modelMatrix(1.0f), _worldMatrix(1.0f), _toBeRemoved(false), _flipX(1.f),
	_scale(1.f), _position(0.f, 0.f, 0.f) {
	updateModelMatrix();
}

void Node::updateModelMatrix() {
	_modelMatrix = glm::mat4(1.0f);

	// scale+flip (X includes flip)
	_modelMatrix = glm::scale(_modelMatrix, glm::vec3(_flipX * _scale, _scale, 1.0f));

	// translation
	_modelMatrix[3] = glm::vec4(_position, 1.0f);
}

void Node::setPosition(glm::vec3 pos) {
	_position = pos;
	updateModelMatrix();
	notifyMove();
}

void Node::setFlipX(bool flip) {
	_flipX = (flip ? -1.f : 1.f);
	updateModelMatrix();
	notifyMove();
}

void Node::setScale(float scale) {
	_scale = scale;
	updateModelMatrix();
	notifyMove();
}

/*
 * Converts world position to local position
 */
glm::vec2 Node::worldToLocal(glm::vec2 worldPos) const {
	glm::vec3 wPos(_worldMatrix[3]);
	float wScale = fabs(_worldMatrix[0][0]);
	float wf = (_worldMatrix[0][0] >= 0.f ? 1.f : -1.f);
	glm::vec2 localPos;
	localPos.x = (worldPos.x - wPos.x) / (wScale * wf);
	localPos.y = (worldPos.y - wPos.y) / wScale;
	return localPos;
}


void Node::remove() {
	_toBeRemoved = true;
}

void Node::add(std::shared_ptr<Node> child) {
	child->_parent = this;
	_children.push_back(child);
}

void Node::addComponent(std::shared_ptr<Component> component) {
	_components.push_back(component);
	component->setNode(this);
	component->start();
}

glm::vec3 Node::getWorldPosition() const {
	return glm::vec3(_worldMatrix[3]);
}

Transform Node::getWorldTransform() const {
	Transform t;
	t.position = glm::vec3(_worldMatrix[3]);
	t.scale = fabs(_worldMatrix[0][0]);
	t.flipX = (_worldMatrix[0][0] >= 0.f ? 1.f : -1.f);
	return t;
}

const glm::mat4 &Node::getWorldMatrix() const {
	return _worldMatrix;
}



void Node::move(glm::vec2 dx) {
	float fx = (_worldMatrix[0][0] >= 0.f ? 1.f : -1.f);
	_position += glm::vec3(fx * dx.x, dx.y, 0.f);
	updateModelMatrix();
	//_modelMatrix = glm::translate(glm::vec3(dx, 0.f)) * _modelMatrix;
	notifyMove();
}

bool Node::lookRight() const {
	return _modelMatrix[0][0] >= 0.f;
}

void Node::setModelMatrix(const Matrix &t) {
	_modelMatrix = t.getMatrix();
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

	// Update components ...
	for (auto& c : _components) {
		c->update(dt);
	}

	customUpdate(dt);

	// ... update model (if any) ...
	if (_model) _model->update();

	// ... and then update children
	for (auto& child : _children) {
		child->update(dt);
	}

}

void Node::customUpdate(double dt) {}

void Node::draw(IShader* shader) {
	if (_model != nullptr) {
		_model->draw(shader);
	}
	for (auto& child : _children) {
		child->draw(shader);
	}
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
	for (auto& obs : _observers) {
		obs->onMove(this);
	}
}


void Node::registerObserver(NodeObserver * observer) {
	_observers.push_back(observer);

}

void Node::unregisterObserver(NodeObserver * observer) {
	_observers.erase(
			std::remove(_observers.begin(), _observers.end(), observer),
			_observers.end()
	);
}

Bounds Node::transformBounds(const Bounds &localBounds) const {
	auto t = getWorldTransform();
	Bounds b = t.transform(localBounds);
	return b;
}