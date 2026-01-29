#pragma once


#include <vector>
#include <cmath>
#include <cstdint>
#include "graph.h"
#include "gamelib/tex.h"

struct Point {
	int x;
	int y;

	bool operator==(const Point& other) const {
		return x == other.x && y == other.y;
	}
};

struct PointHash {
	std::size_t operator()(const Point& p) const {
		return (p.x * 73856093) ^ (p.y * 19349663);
	}
};

class GridGraph : public Graph<Point> {
public:
	GridGraph(const Tex& tex) : _width(tex.getSize().x), _height(tex.getSize().y), _tex(tex) {}

	void neighbors(const Point& p, std::vector<Point>& out) const override {
		static const int dirs[8][2] = {
				{1,0},{-1,0},{0,1},{0,-1},
				{1,1},{1,-1},{-1,1},{-1,-1}
		};

		for (auto& d : dirs) {
			Point n{p.x + d[0], p.y + d[1]};
			if (d[0] == 0 || d[1] == 0) {
				if (inside(n) && walkable(n))
					out.push_back(n);
			} else {
				// diagonal move, check both adjacent squares
				Point adj1{p.x + d[0], p.y};
				Point adj2{p.x, p.y + d[1]};
				if (inside(n) && walkable(n) && walkable(adj1) && walkable(adj2))
					out.push_back(n);
			}
		}
	}

	float cost(const Point& p1, const Point& p2) const override {
		return (std::abs(p1.x - p2.x) + std::abs(p1.y - p2.y) == 1) ? 1.0f : 1.4142f; // 1 for orthogonal, sqrt(2) for diagonal
	}

	float heuristic(const Point& a, const Point& b) const override {
		return std::abs(a.x - b.x) + std::abs(a.y - b.y); // Manhattan
	}

private:
	bool inside(const Point& p) const {
		return p.x >= 0 && p.y >= 0 && p.x < _width && p.y < _height;
	}

	bool walkable(const Point& p) const {
		return _tex.getIndex(p.x, _height - 1 - p.y) != 0;
	}

	int _width, _height;
	const Tex& _tex;
};
