#include <random>
#include <time.h>

#ifndef __RAND_H__
#define __RAND_H__

class Rand {
	private:
		std::default_random_engine generator;
		std::normal_distribution<float> distribution;

	public:
		Rand() {
			// TODO: To make sure that we get the same results to debug easier
			generator = std::default_random_engine(0);//(unsigned int)time(0));
			distribution = std::normal_distribution<float>(0, 1);
		}

		float NormFloat64() {
			return distribution(generator);
		}

		int Intn(int bounds) {
			int value = rand();
			return value % bounds;
		}
};

#endif