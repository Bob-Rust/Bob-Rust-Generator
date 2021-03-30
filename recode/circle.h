#ifndef __CIRCLE_H_
#define __CIRCLE_H_

#include <sstream>
#include "worker.h"
#include "../utils.h"

class Circle {
	private:
		Worker* worker = 0;

	public:
		int x, y, r;

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

		char* BORST(char* attrs) {
			std::stringstream stream;
			stream << x << "," << y << "," << SIZE_INDEX(r) << attrs;

			std::string str(stream.str());
			return (char*)str.c_str();
		}

		void Mutate() {
			int w = worker->w;
			int h = worker->h;
			Rand* rnd = worker->rnd;

			switch(rnd->Intn(3)) {
				case 0: {
					int a = x + (int)(rnd->NormFloat64() * 16);
					int b = y + (int)(rnd->NormFloat64() * 16);
					x = clampInt(a, 0, w - 1);
					y = clampInt(b, 0, h - 1);
				}
				case 1:
				case 2: {
					int c = closestSize(r + (int)(rnd->NormFloat64() * 16));
					r = clampInt(c, 1, w-1);
				}
			}
		}

		vector<Scanline> Rasterize() {
			int w = worker->w;
			int h = worker->h;
			
			vector<Scanline> list = worker->lines;
			for(int dy = 0; dy < r; dy++) {
				int y1 = y - dy;
				int y2 = y + dy;

				// TODO: Check if this is needed
				if((y1 < 0 || y1 >= h) && (y2 < 0 || y2 >= h)) {
					continue;
				}

				int s = (int)(sqrt(r * r - dy * dy));
				int x1 = x - s;
				int x2 = x + s;
				if(x1 < 0) x1 = 0;
				if(x2 >= w) x2 = w - 1;

				if(y1 >= 0 && y1 < h) {
					Scanline line{y1, x1, x2};
					list.push_back(line);
				}

				if(y2 >= 0 && y2 < h && dy > 0) {
					Scanline line{y2, x1, x2};
					list.push_back(line);
				}
			}

			return list;
		}
};

#endif
