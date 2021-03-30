#pragma once

class Annealable {
	public:
		virtual ~Annealable() {}
		virtual float Energy() = 0;
		virtual void UndoMove(Annealable* state) = 0;
		
		// You need to call delete on objects returned from this method
		virtual Annealable* DoMove() = 0;
		// You need to call delete on objects returned from this method
		virtual Annealable* Copy() = 0;
};

// You need to call delete on objects returned from this method
Annealable* HillClimb(Annealable* state, int maxAge) {
	state = state->Copy();

	Annealable* bestState = state->Copy();
	float minimumEnergy = state->Energy();

	// This function will minimize the energy of the input state

	int step = 0;
	for(int age = 0; age < maxAge; age++) {
		Annealable* undo = state->DoMove();
		float energy = state->Energy();

		if(energy >= minimumEnergy) {
			// Changes the old state
			state->UndoMove(undo);
		} else {
			//printf("step: %d, energy: %.6f\n", step, energy);
			minimumEnergy = energy;

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

	// This method creates (1) new reference of (Annealable*)
	return bestState;
}
