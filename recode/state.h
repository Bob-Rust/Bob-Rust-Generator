#ifndef __STATE_H__
#define __STATE_H__

#include "../utils.h"
#include "optimize.h"
#include "circle.h"
#include "rand.h"

// TODO: Remove all state pointers and replace with non pointer stuff
//       If we remove the pointer from all states we would not need to
//       call delete on every single one. This could save both memory
//       and time.
//       Investigate if this is possible.

// 32 bytes per state, (+ vtable)
class State : public Annealable {
	private:
		// [Legacy]
		bool mutateAlpha;

	public:
		Worker* worker;
		Circle shape;
		float score;
		int alpha;

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

		virtual float Energy() {
			if(score < 0) {
				vector<Scanline> list = shape.Rasterize();

				score = worker->Energy(list, alpha);
			}

			return score;
		}

		virtual State* DoMove() {
			State* oldState = Copy();
			shape.Mutate();

			if(mutateAlpha) {
				int val = alpha + worker->rnd->Intn(21) - 10;
				alpha = clampInt(val, 1, 255);
			}

			score = -1;
			return oldState;
		}

		virtual void UndoMove(Annealable* state) {
			State* oldState = (State*)state;
			shape = oldState->shape;
			alpha = oldState->alpha;
			score = oldState->score;
		}

		virtual State* Copy() {
			return new State(worker, shape, alpha, mutateAlpha, score);
		}
};

#endif
