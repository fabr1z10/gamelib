#pragma once

#include <random>

class Random {
private:
	Random();
public:
	Random(const Random&) = delete;

	Random& operator=(const Random&) = delete;

	static Random& instance() {
		static Random instance;
		return instance;
	}

	// Get integer in [min, max]
	int nextInt(int min, int max);

	// Get floating-point in [min, max)
	double nextDouble(double min = 0.0, double max = 1.0);

private:
	std::mt19937 _rng;
};