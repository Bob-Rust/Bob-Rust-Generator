#include "all.h"
#include <math.h>

#ifndef __CORE_H__
#define __CORE_H__

#include "../utils.h"
#include "bundle.h"

typedef long long int64;
typedef unsigned long long uint64;

Color computeColor(Image* target, Image* current, vector<Scanline>& lines, int alpha) {
	int a = 257 * 255 / alpha;

	int64 rsum = 0;
	int64 gsum = 0;
	int64 bsum = 0;
	int count = 0;

	for(unsigned int i = 0; i < lines.size(); i++) {
		Scanline line = lines[i];
		int idx = target->PixOffset(line.x1, line.y);

		for(int x = line.x1; x <= line.x2; x++) {
			Color tt = target->Pix[idx];
			Color cc = current->Pix[idx];
			idx++;

			rsum += ((int64)((int)tt.r - (int)cc.r) * a + ((int64)cc.r * 257));
			gsum += ((int64)((int)tt.g - (int)cc.g) * a + ((int64)cc.g * 257));
			bsum += ((int64)((int)tt.b - (int)cc.b) * a + ((int64)cc.b * 257));
			count++;
		}
	}

	// TODO: This could be bad!
	if(!count) return Color{0,0,0,0};

	int r = (int)(rsum / count) >> 8;
	int g = (int)(gsum / count) >> 8;
	int b = (int)(bsum / count) >> 8;
	r = clampInt(r, 0, 255);
	g = clampInt(g, 0, 255);
	b = clampInt(b, 0, 255);

	Color result{(unsigned char)r, (unsigned char)g, (unsigned char)b, (unsigned char)alpha};
	result = closestColor(result);
	return result;
}

void copyLines_replaceRegion(Image* dst, Image* src, vector<Scanline>& lines) {
	for(unsigned int i = 0; i < lines.size(); i++) {
		Scanline line = lines[i];

		int idx = dst->PixOffset(line.x1, line.y);

		// TODO: Use memcpy
		for(int x = line.x1; x < line.x2; x++) {
			dst->Pix[idx] = src->Pix[idx];
			idx++;
		}
	}
}

void drawLines(Image* im, Color& c, vector<Scanline>& lines) {
	for(unsigned int i = 0; i < lines.size(); i++) {
		Scanline line = lines[i];
		int idx = im->PixOffset(line.x1, line.y);

		for(int x = line.x1; x <= line.x2; x++) {
			Color& a = im->Pix[idx++];
			a.r = ((c.r * c.a) + (a.r * (255 - c.a))) / 255;
			a.g = ((c.g * c.a) + (a.g * (255 - c.a))) / 255;
			a.b = ((c.b * c.a) + (a.b * (255 - c.a))) / 255;
			a.a = 255 - (((255 - a.a) * (255 - c.a)) / 255);
		}
	}
}

// [Only used once]
float differenceFull(Image* a, Image* b) {
	int w = a->width;
	int h = a->height;

	uint64 total = 0;
	for(int y = 0; y < h; y++) {
		int idx = a->PixOffset(0, y);

		for(int x = 0; x < w; x++) {
			Color aa = a->Pix[idx];
			Color bb = b->Pix[idx];
			idx++;

			uint64 dr = (int)aa.r - (int)bb.r;
			uint64 dg = (int)aa.g - (int)bb.g;
			uint64 db = (int)aa.b - (int)bb.b;
			uint64 da = (int)aa.a - (int)bb.a;
			total += dr*dr + dg*dg + db*db + da*da;
		}
	}

	// We divide with * 4.0f because we add 4 numbers above
	return sqrt(total / (w * h * 4.0f)) / 255.0f;
}

float differencePartial(Image* target, Image* before, Image* after, float score, vector<Scanline>& lines) {
	int w = target->width;
	int h = target->height;
	int64 total = (int64)(pow(score * 255, 2) * (w * h * 4.0f));

	for(unsigned int i = 0; i < lines.size(); i++) {
		Scanline line = lines[i];
		int idx = target->PixOffset(line.x1, line.y);

		for(int x = line.x1; x <= line.x2; x++) {
			Color tt = target->Pix[idx];
			Color bb = before->Pix[idx];
			Color aa = after->Pix[idx];
			idx++;
			
			int64 dr1 = (int)tt.r - (int)bb.r;
			int64 dg1 = (int)tt.g - (int)bb.g;
			int64 db1 = (int)tt.b - (int)bb.b;
			int64 da1 = (int)tt.a - (int)bb.a;
			int64 dr2 = (int)tt.r - (int)aa.r;
			int64 dg2 = (int)tt.g - (int)aa.g;
			int64 db2 = (int)tt.b - (int)aa.b;
			int64 da2 = (int)tt.a - (int)aa.a;
			total -= int64(dr1*dr1 + dg1*dg1 + db1*db1 + da1*da1);
			total += int64(dr2*dr2 + dg2*dg2 + db2*db2 + da2*da2);
		}
	}
	
	return sqrt(total / (w * h * 4.0f)) * 0.003921568627451f; // 255
}

#endif