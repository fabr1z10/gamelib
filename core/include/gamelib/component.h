#pragma once

class Node;

class Component {
public:
	Component() : _active(true), _node(nullptr) {}

	virtual ~Component() = default;

	virtual void start() {}

	virtual void update(double) {}

	virtual void setNode(Node*);

	Node* getNode();

	bool isActive() const;

	void setActive(bool);

protected:
	Node* _node;
	bool _active;

};


inline Node* Component::getNode() {
	return _node;
}

inline void Component::setNode(Node* node) {
	_node = node;
}



inline bool Component::isActive() const {
	return _active;
}

inline void Component::setActive(bool value) {
	_active = value;
}

