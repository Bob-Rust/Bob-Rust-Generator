#ifndef __CIRCLE_H_
#define __CIRCLE_H_

#include <sstream>

#include "shape.h"
#include "worker.h"
#include "rand.h"
#include "../utils.h"

class Circle : public Shape {
	public:
		Worker* worker = 0;
		int x, y, r;

		Circle(Worker* worker) {
			this->worker = worker;

			Rand* rnd = worker->rnd;
			this->x = rnd->Intn(worker->w);
			this->y = rnd->Intn(worker->h);
			this->r = ARR_SIZES[rnd->Intn(NUM_SIZES)];// * paintingToolScale];
		}

		Circle(Worker* worker, int x, int y, int r) {
			this->worker = worker;
			this->x = x;
			this->y = y;
			this->r = r;
		}

		virtual void Draw(Image* dc, Color c) {
			vector<Scanline> lines = Rasterize();

			for(unsigned int i = 0; i < lines.size(); i++) {
				Scanline line = lines[i];
				int idx = dc->PixOffset(line.x1, line.y);

				for(int x = line.x1; x <= line.x2; x++) {
					Color& a = dc->Pix[idx++];

					// color = (next.color * (next.alpha) + last.color * (255 - next.alpha)) / 255;
					a.r = ((c.r * c.a) + (a.r * (255 - c.a))) / 255;
					a.g = ((c.g * c.a) + (a.g * (255 - c.a))) / 255;
					a.b = ((c.b * c.a) + (a.b * (255 - c.a))) / 255;
					a.a = 255 - (((255 - a.a) * (255 - c.a)) / 255);
				}
			}
		}

		virtual char* BORST(char* attrs) {
			std::stringstream stream;
			stream << x << "," << y << "," << SIZE_INDEX(r) << attrs;

			std::string str(stream.str());
			return (char*)str.c_str();
		}

		virtual Circle* Copy() {
			return new Circle(worker, x, y, r);
		}

		virtual void Mutate() {
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

		virtual vector<Scanline> Rasterize() {
			int w = worker->w;
			int h = worker->h;
			
			vector<Scanline> list = worker->lines;

			for(int dy = 0; dy < r; dy++) {
				int y1 = y - dy;
				int y2 = y + dy;

				// TODO: This is maybe not needed???
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