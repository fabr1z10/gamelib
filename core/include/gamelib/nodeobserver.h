#pragma once

class Node;


class NodeObserver {

public:
	virtual void onMove(Node*) = 0;

	virtual void onRemove(Node*) = 0;
};