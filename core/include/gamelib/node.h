#pragma once

#include <vector>
#include <memory>
#include <glm/glm.hpp>
#include "gamelib/bounds.h"
#include "gamelib/transform.h"
#include "gamelib/component.h"
#include "gamelib/nodeobserver.h"

class IShader;

class IModel;

class Node {

public:
	Node();

	// setters
	void setPosition(glm::vec3);

	void setFlipX(bool flip);

	void setScale(float);

	glm::vec2 worldToLocal(glm::vec2 worldPos) const;
	/*
	 * Adds a child node
	 */
	void add(std::shared_ptr<Node> child);

	/*
	 * Adds a component to this node
	 */
	void addComponent(std::shared_ptr<Component> component);

	void update(double dt);

	void remove();

	void draw(IShader*);

	Bounds transformBounds(const Bounds& localBounds) const;

	glm::vec3 getWorldPosition() const;

	Transform getWorldTransform() const;

	const glm::mat4& getWorldMatrix() const;





	void setModelMatrix(const Matrix& t);

	void setModel(std::shared_ptr<IModel> model);


	IModel* getModel() const { return _model.get(); }

	bool lookRight() const;

	void move(glm::vec2 delta);

	void registerObserver(NodeObserver*);

	void unregisterObserver(NodeObserver*);


protected:
	void updateModelMatrix();
	glm::vec3 _position;
	float _scale;
	float _flipX;   // ±1
	bool _toBeRemoved;
	// this is -1 if flipped, 1 otherwise
	void notifyMove();
	std::vector<std::shared_ptr<Component>> _components;
	std::vector<std::shared_ptr<Node>> _children;
	std::shared_ptr<IModel> _model;
	Node* _parent;
	glm::mat4 _modelMatrix;
	glm::mat4 _worldMatrix;
	std::vector<NodeObserver*> _observers;
};

