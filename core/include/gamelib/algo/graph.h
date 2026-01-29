#pragma once

#include <vector>

template<typename T>
class Graph {

public:
	// returns the neighbors of a node
	virtual void neighbors(const T& node, std::vector<T>& outNeighbors) const = 0;

	// Cost to move from a -> b
	virtual float cost(const T& a, const T& b) const = 0;


	// Heuristic estimate from a -> goal
	virtual float heuristic(const T& a, const T& goal) const = 0;

	virtual ~Graph() = default;

};