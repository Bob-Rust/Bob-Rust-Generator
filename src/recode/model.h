#pragma once

#include "bundle.h"
#include "circle.h"
#include "worker.h"
#include "core.h"
#include "util.h"

#ifndef __MODEL_H__
#define __MODEL_H__

#include <vector>
using std::vector;


class Model {
	private:
		Worker* worker;
		Image* target;
		Image* context;
		int alpha;
		
	public:
		vector<Circle> shapes;
		vector<Color> colors;
		Image* current;
		float score;
		int width;
		int height;

		Model(Image* target, Color background, int alpha) {
			int w = target->width;
			int h = target->height;
			this->target = target;
			this->width = w;
			this->height = h;

			this->current = new Image(w, h);
			for(int i = 0; i < w * h; i++) {
				current->Pix[i] = background;
			}
			
			this->score = differenceFull(target, current);
			this->context = new Image(w, h);
			this->worker = new Worker(target, alpha);
			this->alpha = alpha;
		}

		~Model() {
			colors.clear();
			shapes.clear();
			
			delete worker;
			delete context;
			delete current;
		}

		void Add(Circle shape) {
			Image* before = copyRGBA(current);

			vector<Scanline> lines = shape.Rasterize();
			Color color = computeColor(target, current, lines, alpha);

			drawLines(current, color, lines);
			float sc = differencePartial(target, before, current, score, lines);
			delete before;

			this->score = sc;
			shapes.push_back(shape);
			colors.push_back(color);

			drawLines(context, color, lines);
		}

		int Step() {
			worker->Init(current, score);
			State state = BestHillClimbState(worker, 1000, 100, 1);
			Add(state.shape);

			return worker->counter;
		}
};

#endif
