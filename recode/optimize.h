#pragma once
#include "log.h"

class Annealable {
	public:
		virtual float Energy() = 0;
		virtual void UndoMove(Annealable* state) = 0;
		
		// You need to call delete on object returned from this method
		virtual Annealable* DoMove() = 0;
		// You need to call delete on object returned from this method
		virtual Annealable* Copy() = 0;
};

Annealable* HillClimb(Annealable* state, int maxAge) {
	state = state->Copy();

	Annealable* bestState = state->Copy();
	float bestEnergy = state->Energy();

	int step = 0;
	for(int age = 0; age < maxAge; age++) {
		Annealable* undo = state->DoMove();
		float energy = state->Energy();

		if(energy >= bestEnergy) {
			state->UndoMove(undo);
		} else {
			//v("step: %d, energy: %.6f\n", step, energy);
			bestEnergy = energy;

			// Free memory
			delete bestState;
			bestState = state->Copy();
			age = -1;
		}
		
		// Free memory
		delete undo;
		step++;
	}

	// Free memory
	delete state;

	return bestState;
}