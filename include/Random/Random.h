#pragma once

#include <random>

namespace Random {
	static std::mt19937 rng;

	void Init() {
		std::random_device dev;
		rng = std::mt19937(dev());
	}

	int	UniformInt(int lowerLimit, int upperLimit) {
		std::uniform_int_distribution<int> dist(lowerLimit, upperLimit); // distribution in range [lowerLimit, upperLimit]
		return dist(rng);
	}

	double UniformDouble(double lowerLimit, double upperLimit) {
		std::uniform_real_distribution<double> dist(lowerLimit, upperLimit); // distribution in range [lowerLimit, upperLimit]
		return dist(rng);
	}

	double Gaussian(double mean, double stdDeviation) {
		std::normal_distribution<double> dist(mean, stdDeviation);
		return dist(rng);
	}
}