#ifndef __WORKER_FUNC_H__
#define __WORKER_FUNC_H__

#include "state.h"
#include "circle.h"
#include <ppl.h>

/// <param name="worker">The owner of this state</param>
/// <param name="count">The amount of shapes to check</param>
/// <returns>The best state computed from 'count' shapes</returns>
State BestRandomState(Worker* worker, int count) {
	vector<State> ch(count);
	concurrency::parallel_for(size_t(0), size_t(count), [&](int i) {
		State state(worker);
		state.Energy();
		ch[i] = state;
	});

	float bestEnergy = 0;
	State bestState;

	for(int i = 0; i < count; i++) {
		State state = ch[i];
		float energy = state.Energy();

		if(i == 0 || energy < bestEnergy) {
			bestEnergy = energy;
			bestState = state;
		}
	}

	return bestState;
}

State HillClimb(State state, int maxAge) {
	State bestState = state;
	float minimumEnergy = state.Energy();

	// This function will minimize the energy of the input state
	int step = 0;
	for(int age = 0; age < maxAge; age++) {
		State undo = state.DoMove();
		float energy = state.Energy();

		if(energy >= minimumEnergy) {
			state.UndoMove(undo);
		} else {
			minimumEnergy = energy;
			bestState = state;
			age = -1;
		}
		
		step++;
	}

	return bestState;
}

/// <param name="worker">The owner of this state</param>
/// <param name="max_random_states">The amount of random states</param>
/// <param name="age">?</param>
/// <param name="times">The max amount of runns</param>
State BestHillClimbState(Worker* worker, int max_random_states, int age, int times) {
	float bestEnergy = 0;
	State bestState;

	for(int i = 0; i < times; i++) {
		State oldstate = BestRandomState(worker, max_random_states);
		float before = oldstate.Energy();

		State state = HillClimb(oldstate, age);
		float energy = state.Energy();
		
		if(i == 0 || energy < bestEnergy) {
			bestEnergy = energy;
			bestState = state;
		}
	}

	return bestState;
}

#endif
