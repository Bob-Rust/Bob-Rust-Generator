#pragma once

#include "bundle.h"

typedef unsigned long long uint64;

class Heatmap {
	public:
		int W, H;
		uint64* Count;

		Heatmap(int w, int h) {
			this->W = w;
			this->H = h;
			this->Count = new uint64[w * h];
		}

		~Heatmap() {
			delete[] Count;
		}

		void Clear() {
			// TODO: Use memset to do this operation
			for(int i = 0; i < W * H; i++) {
				Count[i] = 0;
			}
		}

		void Add(vector<Scanline>& lines) {
			for(unsigned int i = 0; i < lines.size(); i++) {
				Scanline line = lines[i];
				int idx = line.y * W + line.x1;

				// TODO: Try use vector operations to apply this to the whole list as fast as possible
				for(int x = line.x1; x <= line.x2; x++) {
					Count[idx++] += 0xffff;
				}
			}
		}

		// [Never Called]
		/*
		void AddHeatmap(Heatmap* a) {
			for(int i = 0; i < a->W * a->H; i++) {
				Count[i] += a->Count[i];
			}
		}
		*/

		// [Never called]
		/*
		func (h *Heatmap) Image(gamma float64) *image.Gray16 {
			im := image.NewGray16(image.Rect(0, 0, h.W, h.H))
			var hi uint64
			for _, h := range h.Count {
				if h > hi {
					hi = h
				}
			}
			i := 0
			for y := 0; y < h.H; y++ {
				for x := 0; x < h.W; x++ {
					p := float64(h.Count[i]) / float64(hi)
					p = math.Pow(p, gamma)
					im.SetGray16(x, y, color.Gray16{uint16(p * 0xffff)})
					i++
				}
			}
			return im
		}
		*/
};