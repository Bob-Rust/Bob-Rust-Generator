#pragma once

#include "all.h"

#ifndef __WORKER_H__
#define __WORKER_H__

#include <vector>
using std::vector;

class Worker {
	public:
		int w, h;
		Image* target;
		Image* current = 0;
		Image* buffer;
		vector<Scanline> lines;
		Rand* rnd;
		float score = 0;
		int counter = 0;

		Worker(Image* target) {
			int w = target->width;
			int h = target->height;
			this->w = w;
			this->h = h;
			this->target = target;
			this->buffer = new Image(w, h);
			this->lines.reserve(4096);
			this->rnd = new Rand();
		}

		~Worker() {
			delete buffer;
			delete rnd;
			lines.clear(); // TODO: Delete
		}

		void Init(Image* current, float score) {
			this->current = current;
			this->score = score;
			this->counter = 0;
		}

		float Energy(Shape* shape, int alpha) {
			this->counter++;
			vector<Scanline> lines = shape->Rasterize();
			
			Color color = computeColor(this->target, this->current, lines, alpha);

			// Set the buffers region on the lines to the current image
			copyLines_replaceRegion(this->buffer, this->current, lines);

			// Draw the lines with the new color to the buffer
			drawLines(this->buffer, color, lines);

			// Get the difference over the drawn region compared to the current with the lines
			float result = differencePartial(this->target, this->current, this->buffer, this->score, lines);
			
			// v("Energy: %.6f\n", result);

			return result;
		}
};

#endif