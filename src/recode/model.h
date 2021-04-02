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
		Color background;
		int sw, sh;
		
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

			this->sw = w;
			this->sh = h;
			this->background = background;
			this->target = target;

			// Create a new image with the specified background color
			this->current = new Image(target->width, target->height);
			for(int i = 0; i < w * h; i++) {
				current->Pix[i] = background;
			}
			
			this->score = differenceFull(target, current);
			this->context = new Image(target->width, target->height);

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

		char* BORST() {
			stringstream stream;

			stream << sw << "," << sh << "\n";

			for(unsigned int i = 0; i < shapes.size(); i++) {
				Circle& shape = shapes[i];
				int color_index = closestColorIndex(colors[i]);

				stream << shape.x << "," << shape.y << "," << SIZE_INDEX(shape.r) << color_index << "\n";
			}

			std::string str(stream.str());

			// TODO: Is this safe?
			return (char*)str.c_str();
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

		int Step(int alpha, int repeat) {
			State state = runWorkers(alpha, 1000, 100, 16);
			
			if(repeat > 0) {
				State last = state;
				for(int i = 0; i < repeat; i++) {
					// Get the worker state and initialize new values
					last.worker->Init(current, score);
					float a = last.Energy();

					// Add the last state to the unload vector
					State next = HillClimb(last, 100);
					float b = next.Energy();
					
					if(a != b) {
						state.shape = next.shape;
						state.alpha = next.alpha;
						// Add(next.shape, next.alpha);
					}

					last = next;
				}
			}

			Add(state.shape, state.alpha);

			return this->worker->counter;
		}

		State runWorkers(int alpha, int max_random_iter, int age, int m) {
			Worker* worker = this->worker;
			worker->Init(current, score);

			// Because we are only using 1 shape. Creating multiple workers is not beneficial
			return runWorker(worker, alpha, max_random_iter, age, 1);
		}

		State runWorker(Worker* worker, int alpha, int max_random_iter, int age, int max_climb_iter) {
			return BestHillClimbState(worker, alpha, max_random_iter, age, max_climb_iter);
		}
};

#endif
