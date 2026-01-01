#include "gamelib/random.h"

Random::Random() : _rng(std::random_device{}()) {

}

int Random::nextInt(int min, int max) {
	std::uniform_int_distribution<int> dist(min, max);
	return dist(_rng);
}

double Random::nextDouble(double min, double max) {
	std::uniform_real_distribution<double> dist(min, max);
	return dist(_rng);
}