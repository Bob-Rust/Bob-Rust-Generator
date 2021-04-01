#ifndef __STATE_H__
#define __STATE_H__

#include "../utils.h"
#include "circle.h"
#include "rand.h"

// 32 bytes per state, (+ vtable)
class State {
	private:
		// [Legacy]
		bool mutateAlpha;

	public:
		Worker* worker;
		Circle shape;
		float score;
		int alpha;
		
		State() {}
		State(Worker* worker, int alpha) : shape(worker) {
			this->worker = worker;
			this->alpha = (alpha == 0 ? 128:alpha);
			this->mutateAlpha = (alpha == 0);
			this->score = -1;
		}

		State(Worker* worker, Circle& sh, int alpha, bool mutateAlpha, float score) : shape(sh) {
			this->worker = worker;
			this->alpha = alpha;
			this->mutateAlpha = mutateAlpha;
			this->score = score;
		}

		~State() {
			worker = 0;
		}

		float Energy() {
			if(score < 0) {
				vector<Scanline> list = shape.Rasterize();

				score = worker->Energy(list, alpha);
			}

			return score;
		}

		State DoMove() {
			State oldState = Copy();
			shape.Mutate();

			if(mutateAlpha) {
				int val = alpha + worker->rnd->Intn(21) - 10;
				alpha = clampInt(val, 1, 255);
			}

			score = -1;
			return oldState;
		}

		void UndoMove(State oldState) {
			shape = oldState.shape;
			alpha = oldState.alpha;
			score = oldState.score;
		}

		// Create a copy that wont generate a new pointer
		State Copy() {
			return State(worker, shape, alpha, mutateAlpha, score);
		}
};

State HillClimb(State inp, int maxAge) {
	State state = inp;

	State bestState = inp;
	float minimumEnergy = state.Energy();

	// This function will minimize the energy of the input state
	int step = 0;
	for(int age = 0; age < maxAge; age++) {
		State undo = state.DoMove();
		float energy = state.Energy();

		if(energy >= minimumEnergy) {
			// Changes the old state
			
			state.UndoMove(undo);
		} else {
			//printf("step: %d, energy: %.6f\n", step, energy);
			minimumEnergy = energy;
			bestState = state;
			age = -1;
		}
		
		step++;
	}

	return bestState;
}

#endif
