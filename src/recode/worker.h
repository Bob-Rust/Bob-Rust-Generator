#pragma once

#include "all.h"

#ifndef __WORKER_H__
#define __WORKER_H__

#include "bundle.h"
#include <vector>
using std::vector;

class Worker {
	private:
		Image* buffer;
		Image* target;
		Image* current = 0;

	public:
		int w, h;
		Rand* rnd;
		float score = 0;
		int counter = 0;
		int alpha;

		Worker(Image* target, int alpha) {
			this->w = target->width;
			this->h = target->height;
			this->target = target;
			this->buffer = new Image(w, h);
			this->rnd = new Rand();
			this->alpha = alpha;
		}

		~Worker() {
			delete buffer;
			delete rnd;
		}

		void Init(Image* current, float score) {
			this->current = current;
			this->score = score;
			this->counter = 0;
		}

		float Energy(vector<Scanline>& lines) {
			this->counter++;

			// Because this is called in parallel we could mess up the buffer? by writing to it twize?
			
			Color color = computeColor(target, current, lines, alpha);

			// Set the buffers region on the lines to the current image
			copyLines_replaceRegion(buffer, current, lines);

			// Draw the lines with the new color to the buffer
			drawLines(buffer, color, lines);
			
			// Get the difference over the drawn region compared to the current with the lines
			return differencePartial(target, current, buffer, score, lines);
		}
};

#endif
