#pragma once

#include "gamelib/algo/graph.h"
#include <queue>
#include <unordered_map>
#include <algorithm>

template<typename Node, typename Hash>
bool astar(
		const Graph<Node>& graph,
		const Node& start,
		const Node& goal,
		std::vector<Node>& outPath
) {
	struct Item {
		Node node;
		float f;
		bool operator>(const Item& o) const { return f > o.f; }
	};

	std::priority_queue<Item, std::vector<Item>, std::greater<Item>> open;

	std::unordered_map<Node, float, Hash> gScore;
	std::unordered_map<Node, Node, Hash> cameFrom;

	gScore[start] = 0.0f;
	open.push({start, graph.heuristic(start, goal)});

	// ---- NEW: best-so-far tracking ----
	Node bestNode = start;
	float bestH = graph.heuristic(start, goal);
	// ----------------------------------

	std::vector<Node> neigh;

	while (!open.empty()) {
		Node current = open.top().node;
		open.pop();

		// ---- NEW: update best-so-far ----
		float h = graph.heuristic(current, goal);
		if (h < bestH) {
			bestH = h;
			bestNode = current;
		}
		// --------------------------------

		if (current == goal) {
			// reconstruct path
			outPath.clear();
			for (Node n = goal; !(n == start); n = cameFrom[n])
				outPath.push_back(n);
			outPath.push_back(start);
			std::reverse(outPath.begin(), outPath.end());
			return true;
		}

		neigh.clear();
		graph.neighbors(current, neigh);

		for (auto& n : neigh) {
			float tentative = gScore[current] + graph.cost(current, n);

			if (!gScore.count(n) || tentative < gScore[n]) {
				cameFrom[n] = current;
				gScore[n] = tentative;
				float f = tentative + graph.heuristic(n, goal);
				open.push({n, f});
			}
		}
	}

	// ---- NEW: fallback path ----
	if (!(bestNode == start)) {
		outPath.clear();
		for (Node n = bestNode; !(n == start); n = cameFrom[n])
			outPath.push_back(n);
		outPath.push_back(start);
		std::reverse(outPath.begin(), outPath.end());
	}
	// ----------------------------
	return false;
}