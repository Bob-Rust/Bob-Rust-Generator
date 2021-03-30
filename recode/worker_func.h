#ifndef __WORKER_FUNC_H__
#define __WORKER_FUNC_H__

#include "state.h"
#include "circle.h"

/// <param name="worker">The owner of this state</param>
/// <param name="alpha">The alpha value of the new state</param>
/// <param name="count">The amount of shapes to check</param>
/// <returns>The best state computed from 'count' shapes</returns>
State* BestRandomState(Worker* worker, int alpha, int count) {
	float bestEnergy = 0;
	State* bestState = 0;

	for(int i = 0; i < count; i++) {
		State* state = new State(worker, alpha);
		float energy = state->Energy();

		if(i == 0 || energy < bestEnergy) {
			bestEnergy = energy;
			delete bestState;
			bestState = state;
		}

		if(bestState != state) {
			// Free memory
			delete state;
		}
	}

	return bestState;
}

/// <param name="worker">The owner of this state</param>
/// <param name="alpha">The alpha value of the generated state</param>
/// <param name="max_random_states">The amount of random states</param>
/// <param name="age">?</param>
/// <param name="times">The max amount of runns</param>
State* BestHillClimbState(Worker* worker, int alpha, int max_random_states, int age, int times) {
	float bestEnergy = 0;
	State* bestState = 0;

	for(int i = 0; i < times; i++) {
		State* oldstate = BestRandomState(worker, alpha, max_random_states);
		float before = oldstate->Energy();

		State* state = (State*)HillClimb((Annealable*)oldstate, age);
		float energy = state->Energy();
		delete oldstate;

		if(i == 0 || energy < bestEnergy) {
			bestEnergy = energy;
			delete bestState;
			bestState = state;
			// printf("[BestHillClimbState]: %dx before: %.6f -> %dx hill climb: %.6f\n", n, before, age, energy);
		}
	}

	// This function returns (1) new reference of (State*)
	return bestState;
}

#endif