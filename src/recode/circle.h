#ifndef __CIRCLE_H_
#define __CIRCLE_H_

#include <sstream>
#include "worker.h"
#include "../utils.h"

// 16 bytes
class Circle {
	private:
		Worker* worker = 0;

	public:
		int x, y, r;
		
		Circle() {}

		Circle(Worker* worker) {
			this->worker = worker;

			Rand* rnd = worker->rnd;
			this->x = rnd->Intn(worker->w);
			this->y = rnd->Intn(worker->h);
			this->r = ARR_SIZES[rnd->Intn(NUM_SIZES)];
		}

		Circle(Worker* worker, int x, int y, int r) {
			this->worker = worker;
			this->x = x;
			this->y = y;
			this->r = r;
		}

		void Mutate() {
			int w = worker->w - 1;
			int h = worker->h - 1;
			Rand* rnd = worker->rnd;

			switch(rnd->Intn(3)) {
				case 0: {
					int a = x + (int)(rnd->NormFloat64() * 16);
					int b = y + (int)(rnd->NormFloat64() * 16);
					x = clampInt(a, 0, w);
					y = clampInt(b, 0, h);
				}
				case 1:
				case 2: {
					int c = closestSize(r + (int)(rnd->NormFloat64() * 16));
					r = clampInt(c, 1, w);
				}
			}
		}

		vector<Scanline> Rasterize() {
			int w = worker->w;
			int h = worker->h;
			
			int cache_index = SIZE_INDEX(r);
			const Scanline* LINES = CIRCLE_CACHE[cache_index];
			const int LENGTH = CIRCLE_CACHE_LENGTH[cache_index];

			int i = r - 1;
			i = (y < i) * (i - y);

			vector<Scanline> list;
			list.reserve((size_t)LENGTH - i);

			Scanline line;
			int yy, x1, x2;
			for(; i < LENGTH; i++) {
				line = LINES[i];
				yy = line.y + y;
				if(yy >= h) return list;
				x1 = line.x1 + x;
				x2 = line.x2 + x;
				x1 = (x1 < 0) ? 0:x1;
				x2 = (x2 >=w) ? (w - 1):x2;
				list.push_back(Scanline{yy, x1, x2});
			}

			return list;
		}
};

#endif
