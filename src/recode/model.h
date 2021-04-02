#pragma once

#ifndef __MODEL_H__
#define __MODEL_H__

#include <sstream>
#include <vector>
#include <stdio.h>
#include <ppl.h>

using std::vector;
using std::stringstream;

#include "bundle.h"
#include "circle.h"
#include "worker.h"
#include "core.h"
#include "util.h"
#include "../utils.h"

class Model {
	private:
		Worker* worker;
		vector<float> scores;
		
	public:
		vector<Circle> shapes;
		vector<Color> colors;
		Image* target;
		Image* current;
		Image* context;
		float score;

		Model(Image* target, Color background) {
			int w = target->width;
			int h = target->height;
			this->target = target;

			this->current = new Image(w, h);
			for(int i = 0; i < w * h; i++) {
				current->Pix[i] = background;
			}
			
			this->score = differenceFull(target, current);
			this->context = new Image(w, h);

			worker = new Worker(target);
		}

		~Model() {
			colors.clear();
			scores.clear();
			shapes.clear();
			
			delete worker;
			delete context;
			delete current;
		}

		void Add(Circle shape, int alpha) {
			Image* before = copyRGBA(current);

			vector<Scanline> lines = shape.Rasterize();
			Color color = computeColor(target, current, lines, alpha);

			drawLines(current, color, lines);
			float sc = differencePartial(target, before, current, score, lines);
			delete before;

			this->score = sc;
			shapes.push_back(shape);
			colors.push_back(color);
			scores.push_back(sc);

			drawLines(context, color, lines);
		}

		int Step(int alpha) {
			worker->Init(current, score);
			State state = BestHillClimbState(worker, alpha, 1000, 100, 1);
			Add(state.shape, state.alpha);
			return this->worker->counter;
		}
};

#endif
