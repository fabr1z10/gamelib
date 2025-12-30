#pragma once

#include <memory>
#include <unordered_map>
#include <utility>
#include <vector>
#include <list>
#include <string>
#include "gamelib/action.h"


class Script {
public:
	explicit Script(const std::string& id = std::string());
	~Script();
	void setLoop();

	bool done() const;

	void addAction(int, const std::shared_ptr<Action>&, const std::vector<int>& previous = {});

	void start();

	std::string id() const;

	void update(double);
private:
	struct Node {
		Node() = default;
		Node(int id, std::shared_ptr<Action> action, std::vector<int> previous) :
			id(id), action(std::move(action)), previous(std::move(previous)) {}
		int id = 0;
		std::shared_ptr<Action> action;
		std::vector<int> next;
		std::vector<int> previous;
	};
	bool _loop;
	std::string _id;
	std::list<int> _current;
	bool _done;
	std::unordered_map<int, std::shared_ptr<Node>> _nodes;
	int _lastAdded;
};

inline std::string Script::id() const {
	return _id;
}
inline bool Script::done() const {
	return _done;
}