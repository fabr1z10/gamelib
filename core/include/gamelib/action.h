#pragma once



class Action {
public:
	Action() : _completed(false) {}

	virtual ~Action() = default;

	virtual void start() = 0;

	virtual void run(double) = 0;

	bool isCompleted() const;

protected:
	bool _completed;
};

inline bool Action::isCompleted() const {
	return _completed;
}