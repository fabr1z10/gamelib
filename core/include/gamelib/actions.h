#pragma once

#include "gamelib/action.h"
#include <functional>

class CallFunc : public Action {
public:
	CallFunc(const std::function<void()>& func) : _func(func) {}

	void start() override {
		_func();
		_completed = true;
	}

	void run(double) override {}
private:
	std::function<void()> _func;
};