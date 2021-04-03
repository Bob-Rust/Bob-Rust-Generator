#ifndef __STATE_H__
#define __STATE_H__

#include "circle.h"

class State {
	private:
		Worker* worker;

	public:
		Circle shape;
		float score;
		
		State() {}
		State(Worker* worker) : shape(worker) {
			this->worker = worker;
			this->score = -1;
		}

		State(Worker* worker, Circle& sh, float score) : shape(sh) {
			this->worker = worker;
			this->score = score;
		}

		~State() {
			worker = 0;
		}

		float Energy() {
			if(score < 0) {
				vector<Scanline> list = shape.Rasterize();
				score = worker->Energy(list);
			}

			return score;
		}

		State DoMove() {
			State oldState = Copy();
			shape.Mutate();

			score = -1;
			return oldState;
		}

		void UndoMove(State oldState) {
			shape = oldState.shape;
			score = oldState.score;
		}

		State Copy() {
			return State(worker, shape, score);
		}
};

#endif
