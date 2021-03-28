#include "../utils.h"
#include "optimize.h"
#include "shape.h"
#include "rand.h"

#ifndef __STATE_H__
#define __STATE_H__

class State : public Annealable {
	public:
		Worker* worker;
		Shape* shape;
		int alpha;
		bool MutateAlpha;
		float score;

		State(Worker* worker, Shape* shape, int alpha) {
			this->worker = worker;
			this->shape = shape;
			this->alpha = (alpha == 0 ? 128:alpha);
			this->MutateAlpha = (alpha == 0);
			this->score = -1;
		}

		State(Worker* worker, Shape* shape, int alpha, bool mutateAlpha, float score) {
			this->worker = worker;
			this->shape = shape;
			this->alpha = alpha;
			this->MutateAlpha = mutateAlpha;
			this->score = score;
		}

		~State() {
			worker = 0;
			delete shape;
		}

		virtual float Energy() {
			if(score < 0) {
				score = worker->Energy(shape, alpha);
			}

			return score;
		}

		virtual State* DoMove() {
			State* oldState = Copy();
			shape->Mutate();

			if(MutateAlpha) {
				int val = alpha + worker->rnd->Intn(21) - 10;
				alpha = clampInt(val, 1, 255);
			}

			score = -1;
			return oldState;
		}

		virtual void UndoMove(Annealable* state) {
			State* oldState = (State*)state;
			delete shape;

			shape = oldState->shape;
			alpha = oldState->alpha;
			score = oldState->score;
		}

		virtual State* Copy() {
			return new State(worker, shape->Copy(), alpha, MutateAlpha, score);
		}
};

#endif