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
		vector<Worker*> workers;
		vector<Circle> shapes;
		vector<Color> colors;
		vector<float> scores;
		Color background;
		float scale;
		int sw, sh;
		
	public:
		Image* target;
		Image* current;
		Image* context;
		float score;


		Model(Image* target, Color background, int size, int numWorkers) {
			int w = target->width;
			int h = target->height;

			float aspect = w / ((float)h);
			int sw = 0;
			int sh = 0;
			float scale = 0;

			if(w >= h) {
				sw = size;
				sh = (int)(size / aspect);
				scale = size / ((float)w);
			} else {
				sw = (int)(size * aspect);
				sh = size;
				scale = size / ((float)h);
			}

			this->sw = sw;
			this->sh = sh;
			this->scale = scale;
			this->background = background;
			this->target = target;

			// Create a new image with the specified background color
			this->current = new Image(target->width, target->height);
			for(int i = 0; i < w * h; i++) {
				current->Pix[i] = background;
			}
			
			this->score = differenceFull(target, current);
			this->context = new Image(target->width, target->height);

			workers.reserve(numWorkers);
			for(int i = 0; i < numWorkers; i++) {
				workers.push_back(new Worker(target));
			}
		}

		~Model() {
			workers.clear();
			colors.clear();
			scores.clear();
			shapes.clear();
			// TODO: Release all workers
			// TODO: Release all shapes

			delete context;
		}

		char* BORST() {
			stringstream stream;

			stream << sw << "," << sh << "\n";

			for(unsigned int i = 0; i < shapes.size(); i++) {
				Circle& sp = shapes[i];

				stringstream atts;
				atts << closestColorIndex(colors[i]);

				std::string str(atts.str());
				stream << sp.BORST((char*)str.c_str()) << "\n";
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
			State* state = runWorkers(alpha, 1000, 100, 16);
			Add(state->shape, state->alpha);

			for(int i = 0; i < repeat; i++) {
				state->worker->Init(current, score);
				float a = state->Energy();

				State* old = state;
				state = (State*)HillClimb(state, 100);
				float b = state->Energy();

				if(a == b) {
					// This is unsafe because this state will be used to seed the last state
					//delete state;

					// This should ensure that we only delete the states that do not end up in memory
					if(i > 0) delete old;
					
					// TODO: Fix this memory leak
					continue;
				}

				Add(state->shape, state->alpha);
			}

			int counter = 0;
			for(unsigned int i = 0; i < workers.size(); i++) {
				counter += workers[i]->counter;
			}

			return counter;
		}

		State* runWorkers(int a, int n, int age, int m) {
			int wn = m / workers.size();
			// wn = m / wn;

			if((m % wn) != 0) {
				wn ++;
			}
			
			wn = 1;
			vector<State*> ch(wn);
			concurrency::parallel_for(size_t(0), size_t(wn), [&](int i) {
				Worker* worker = workers[i];
				worker->Init(current, score);
				ch[i] = runWorker(worker, a, n, age, wn);
			});

			float bestEnergy = 0;
			State* bestState = 0;
			for(unsigned int i = 0; i < ch.size(); i++) {
				State* state = ch[i];
				float energy = state->Energy();
				
				if(i == 0 || energy < bestEnergy) {
					bestEnergy = energy;
					delete bestState;
					bestState = state;
				}

				if(bestState != state) {
					delete state;
				}
			}

			return bestState;
		}

		State* runWorker(Worker* worker, int a, int n, int age, int m) {
			return BestHillClimbState(worker, a, n, age, m);
		}
};

#endif