#ifndef __MODEL_H__
#define __MODEL_H__

#include <sstream>
#include <vector>
using std::vector;
using std::stringstream;

#include "bundle.h"
#include "shape.h"
#include "worker.h"
#include "core.h"
#include "util.h"
#include "../utils.h"

class Model {
	public:
		int sw, sh;
		float scale;
		Color background;
		Image* target;
		Image* current;
		Image* context; //void* Context; // Context    *gg.Context
		float score;

		vector<Shape*> shapes;
		vector<Color> colors;
		vector<float> scores;
		vector<Worker*> workers;

		Model(Image* target, Color background, int size, int numWorkers) {
			int w = target->width;
			int h = target->height;

			float aspect = w / ((float)h);
			int sw = 0;
			int sh = 0;
			float scale = 0;

			if(w >= h) { // if aspect >= 1 {
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
			this->target = target; // model.Target = imageToRGBA(target)

			// Create a new image with the specified background color
			this->current = new Image(target->width, target->height);
			// uniformRGBA(target.Bounds(), background.NRGBA())
			for(int i = 0; i < w * h; i++) {
				current->Pix[i].rgba = background.rgba;
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
				Shape* sp = shapes[i];

				stringstream atts;
				atts << closestColorIndex(colors[i]);

				std::string str(atts.str());
				stream << sp->BORST((char*)str.c_str()) << "\n";
			}

			std::string str(stream.str());
			// TODO: BAD
			return (char*)str.c_str();
		}

		void Add(Shape* shape, int alpha) {
			Image* before = copyRGBA(current);

			vector<Scanline> lines = shape->Rasterize();
			Color color = computeColor(target, current, lines, alpha);
			
			drawLines(current, color, lines);
			float sc = differencePartial(target, before, current, score, lines);
			delete before;

			this->score = sc;
			shapes.push_back(shape);
			colors.push_back(color);
			scores.push_back(sc);

			shape->Draw(context, color);
		}

		int Step(ShapeType shapeType, int alpha, int repeat) {
			State* state = runWorkers(shapeType, alpha, 1000, 100, 16);
			// Bad  memory

			// state = HillClimb(state, 1000).(*State)
			Add(state->shape, state->alpha);

			for(int i = 0; i < repeat; i++) {
				state->worker->Init(current, score);
				float a = state->Energy();

				state = (State*)HillClimb(state, 100);
				float b = state->Energy();

				if(a == b) {
					//delete state;
					continue;
				}

				Add(state->shape, state->alpha);
			}

			// for _, w := range model.Workers[1:] {
			// 	model.Workers[0].Heatmap.AddHeatmap(w.Heatmap)
			// }
			// SavePNG("heatmap.png", model.Workers[0].Heatmap.Image(0.5))

			int counter = 0;
			for(unsigned int i = 0; i < workers.size(); i++) {
				counter += workers[i]->counter;
			}

			return counter;
		}

		State* runWorkers(ShapeType t, int a, int n, int age, int m) {
			int wn = workers.size();
			vector<State*> ch;
			ch.reserve(wn);
			wn = m / wn;

			// What does this even do?
			if((m % wn) != 0) {
				wn ++;
			}

			for(int i = 0; i < wn; i++) {
				Worker* worker = workers[i];
				worker->Init(current, score);

				// This should be run in parallel
				runWorker(worker, t, a, n, age, wn, ch);
			}

			float bestEnergy = 0;
			State* bestState = 0;
			// std::cout << "Size: expected: " << wn << ", got: " << ch.size() << "\n";

			for(unsigned int i = 0; i < ch.size(); i++) {
				State* state = ch[i];
				float energy = state->Energy();
				
				if(i == 0 || energy < bestEnergy) {
					bestEnergy = energy;
					bestState = state;
				}

				if(bestState != state) {
					delete state;
				}
			}

			return bestState;
		}

		void runWorker(Worker* worker, ShapeType t, int a, int n, int age, int m, vector<State*>& ch) {
			// TODO: Make this thread safe
			ch.push_back(BestHillClimbState(worker, t, a, n, age, m));
		}
};

#endif