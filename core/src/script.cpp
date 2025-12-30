#include "gamelib/script.h"
#include <stdexcept>
#include <iostream>

Script::Script(const std::string &id) : _id(id), _loop(false), _done(false), _lastAdded(0) {
	_nodes[0] = std::make_shared<Node>();
}

Script::~Script() {
	std::cout << "SUCCESSFULLY DESTROYED SCRIPT " << _id << "\n";
}

void Script::setLoop() {
	_loop =true;
}

void Script::start() {
	for (const auto& n : _nodes[0]->next) {
		_current.push_back(n);
		_nodes[n]->action->start();
	}
}

void Script::addAction(int id, const std::shared_ptr<Action>& action, const std::vector<int>& previous) {
	if (id == 0) {
		throw std::runtime_error("Cannot add action with id 0");
	}
	if (_nodes.find(id) != _nodes.end()) {
		throw std::runtime_error("Action with id " + std::to_string(id) + " already exists in script " + _id);
	}
	_nodes[id] = std::make_shared<Node>(id, action, previous);
	if (previous.empty()) {
		_nodes[_lastAdded]->next.push_back(id);
	} else {
		for (auto p: previous) {
			_nodes[p]->next.push_back(id);
		}
	}
	_lastAdded = id;
}


void Script::update(double dt) {
	std::vector<int> complete;
	for (auto it = _current.begin(); it != _current.end();) {
		// execute
		int currentId = *it;
		auto& action = _nodes[currentId]->action;
		if (action->isCompleted()) {
			it = _current.erase(it);
			// test for next
			for (auto n : _nodes[currentId]->next) {
				bool add{true};
				for (auto p : _nodes[n]->previous) {
					if (!_nodes[p]->action->isCompleted()) {
						add = false;
						break;
					}
				}
				if (add) {
					_current.push_back(n);
					_nodes[n]->action->start();
				}
			}
		} else {
			action->run(dt);
			it++;
		}
	}

	if (_current.empty()) {
		if (_loop) {
			start();
		} else {
			_done = true;
		}
	}

}