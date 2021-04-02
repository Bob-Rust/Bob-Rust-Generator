#ifndef __RAND_H__
#define __RAND_H__

#include <random>
using std::default_random_engine;
using std::normal_distribution;

#ifdef _SEED_WITH_TIME
#include <time.h>
#endif

class Rand {
	private:
		default_random_engine generator;
		normal_distribution<float> distribution;

	public:
		Rand() : generator(
#ifdef _SEED_WITH_TIME
			time(0)
#else
			0
#endif
		), distribution(0, 1) {}

		float NormFloat64() {
			return distribution(generator);
		}

		int Intn(int bounds) {
			int value = rand();
			return value % bounds;
		}
};

#endif
