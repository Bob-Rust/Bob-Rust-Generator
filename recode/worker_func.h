#include "all.h"

#ifndef __WORKER_FUNC_H__
#define __WORKER_FUNC_H__

#include "state.h"
#include "shape.h"
#include "circle.h"

State* RandomState(Worker* worker, ShapeType t, int a) {
	Shape* shape = new Circle(worker);
	return new State(worker, shape, a);
}

State* BestRandomState(Worker* worker, ShapeType t, int a, int n) {
	float bestEnergy = 0;
	State* bestState = 0;

	for(int i = 0; i < n; i++) {
		State* state = RandomState(worker, t, a);
		float energy = state->Energy();

		if(i == 0 || energy < bestEnergy) {
			bestEnergy = energy;
			bestState = state;
		}

		if(bestState != state) {
			// Free memory
			delete state;
		}
	}

	return bestState;
}

State* BestHillClimbState(Worker* worker, ShapeType t, int a, int n, int age, int m) {
	float bestEnergy = 0;
	State* bestState = 0;

	for(int i = 0; i < m; i++) {
		State* state = BestRandomState(worker, t, a, n);
		float before = state->Energy();
		state = (State*)HillClimb((Annealable*)state, age);
		float energy = state->Energy();
		

		if(i == 0 || energy < bestEnergy) {
			bestEnergy = energy;
			bestState = state;
			vv("[BestHillClimbState]: %dx before: %.6f -> %dx hill climb: %.6f\n", n, before, age, energy);
		}

		if(bestState != state) {
			// Free memory
			delete state;
		}
	}

	return bestState;
}

#endif