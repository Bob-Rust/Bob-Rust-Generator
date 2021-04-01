#include "all.h"
#include <math.h>

#ifndef __CORE_H__
#define __CORE_H__

#include <vector>
using std::vector;

#include "../utils.h"
#include "bundle.h"

typedef long long int64;
typedef unsigned long long uint64;

// This function calculates the best next color with a alpha values
// This alpha check is done using a reverse alpha sum
Color computeColor(Image* target, Image* current, vector<Scanline>& lines, int alpha) {
	// TODO: Calculate the total maximum value that rsum, gsum, bsum could hold and maybe
	//       reduce the size from int64 to int
	int64 rsum = 0;
	int64 gsum = 0;
	int64 bsum = 0;
	int count = 0;
	int w = target->width;

	// This will throw a division by zero exception if alpha is zero
	int pd = (255 << 8) / alpha;
	int pa = (255 - alpha);

	Color tt, cc;
	for(unsigned int i = 0; i < lines.size(); i++) {
		Scanline line = lines[i];
		int idx = line.y * w;
		Color* tta = target->Pix + idx;
		Color* cca = current->Pix + idx;

		for(int x = line.x1; x <= line.x2; x++) {
			tt = tta[x];
			cc = cca[x];
			rsum += int64(((int)tt.r - (int)cc.r) * pd + (cc.r << 8));
			gsum += int64(((int)tt.g - (int)cc.g) * pd + (cc.g << 8));
			bsum += int64(((int)tt.b - (int)cc.b) * pd + (cc.b << 8));
		}

		count += (line.x2 - line.x1 + 1);
	}

	// TODO: Unnecessary check
	if(!count) return Color{0};

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
	int w = dst->width;

	for(unsigned int i = 0; i < lines.size(); i++) {
		Scanline line = lines[i];
		int idx = line.x1 + line.y * w;
		
		memcpy(dst->Pix + idx, src->Pix + idx, (line.x2 - line.x1 + 1) * sizeof(Color));
	}
}

void drawLines(Image* im, Color& c, vector<Scanline>& lines) {
	int cr = (c.r * c.a);
	int cg = (c.g * c.a);
	int cb = (c.b * c.a);
	int pa = (255 - c.a);
	int w = im->width;

	for(unsigned int i = 0; i < lines.size(); i++) {
		Scanline line = lines[i];
		Color* ima = im->Pix + line.y * w;

		for(int x = line.x1; x <= line.x2; x++) {
			Color& a = ima[x];
			a.r = (cr + (a.r * pa)) >> 8;
			a.g = (cg + (a.g * pa)) >> 8;
			a.b = (cb + (a.b * pa)) >> 8;
			a.a = 255 - (((255 - a.a) * pa) >> 8);
		}
	}
}

// [Only used once]
float differenceFull(Image* a, Image* b) {
	int w = a->width;
	int h = a->height;

	uint64 total = 0;
	Color* aaa = a->Pix;
	Color* bba = b->Pix;

	Color aa, bb;
	for(int y = 0; y < h; y++) {
		for(int x = 0; x < w; x++) {
			aa = aaa[x];
			bb = bba[x];
			int dr = (int)aa.r - (int)bb.r;
			int dg = (int)aa.g - (int)bb.g;
			int db = (int)aa.b - (int)bb.b;
			int da = (int)aa.a - (int)bb.a;
			total += uint64(dr*dr + dg*dg + db*db + da*da);
		}

		aaa += w;
		bba += w;
	}

	return sqrt(total / (w * h * 4.0f)) / 255.0f;
}

float differencePartial(Image* target, Image* before, Image* after, float score, vector<Scanline>& lines) {
	int w = target->width;
	int h = target->height;
	float denom = (w * h * 4.0f);
	int64 total = (int64)(pow(score * 255, 2) * denom);
	
	for(unsigned int i = 0; i < lines.size(); i++) {
		Scanline line = lines[i];
		int idx = line.y * w;

		Color* tta = target->Pix + idx;
		Color* bba = before->Pix + idx;
		Color* aaa = after->Pix + idx;

		for(int x = line.x1; x <= line.x2; x++) {
			Color tt = tta[x];
			Color bb = bba[x];
			Color aa = aaa[x];
			
			int dr1 = (int)tt.r - (int)bb.r;
			int dg1 = (int)tt.g - (int)bb.g;
			int db1 = (int)tt.b - (int)bb.b;
			int da1 = (int)tt.a - (int)bb.a;
			int dr2 = (int)tt.r - (int)aa.r;
			int dg2 = (int)tt.g - (int)aa.g;
			int db2 = (int)tt.b - (int)aa.b;
			int da2 = (int)tt.a - (int)aa.a;
			
			total -= int64(dr1*dr1 + dg1*dg1 + db1*db1 + da1*da1);
			total += int64(dr2*dr2 + dg2*dg2 + db2*db2 + da2*da2);
		}
	}
	
	return sqrt(total / denom) * 0.003921568627451f;
}

/*
#include <immintrin.h>
#include <emmintrin.h>
#include <xmmintrin.h>

// 32 bit
static inline __m256 _mm256_abd_ps(__m256 a, __m256 b) {
    return _mm256_sub_ps(_mm256_max_ps(a, b), _mm256_min_ps(a, b));
}
static inline __m256 _load_bytes_to_m256(void* p) {
	__m128i a = _mm_loadu_si128((const __m128i*)p);
	return _mm256_cvtepi32_ps(_mm256_cvtepu8_epi32(a));
}

static inline float _mm256_sum_ps(__m256 x) {
    __m128 hi = _mm256_extractf128_ps(x, 1);
    __m128 lo = _mm256_extractf128_ps(x, 0);
    lo = _mm_add_ps(hi, lo);
    hi = _mm_movehl_ps(hi, lo);
    lo = _mm_add_ps(hi, lo);
    hi = _mm_shuffle_ps(lo, lo, 1);
    lo = _mm_add_ss(hi, lo);
    return _mm_cvtss_f32(lo);
}

float differencePartial(Image* target, Image* before, Image* after, float score, vector<Scanline>& lines) {
	int w = target->width;
	int h = target->height;

	int64 total = (int64)(pow(score * 255, 2) * (w * h * 4.0f));
	int64 told = total;

	int len = 0;
	for(unsigned int i = 0; i < lines.size(); i++) {
		Scanline line = lines[i];
		len += line.x2 - line.x1;
	}

	int tbl = (len + 8) & (~7);
	int* T = new int[tbl];
	int* B = new int[tbl];
	int* A = new int[tbl];
	
	// Only needed for the last 32 elements that is not getting overriden by a value.
	memset(T, 0, tbl * sizeof(int));
	memset(B, 0, tbl * sizeof(int));
	memset(A, 0, tbl * sizeof(int));

	unsigned int midx = 0;
	for(unsigned int i = 0; i < lines.size(); i++) {
		Scanline line = lines[i];

		int idx = line.x1 + line.y * w;
		int csz = line.x2 - line.x1 + 1;
		int czz = csz * sizeof(int);

		memcpy(T + midx, target->Pix + idx, czz);
		memcpy(B + midx, before->Pix + idx, czz);
		memcpy(A + midx, after->Pix + idx, czz);
		midx += csz;
	}

	int stg = tbl >> 3;

	__m256i zero = {0};

	for(int i = 0; i < tbl; i += 8) {
		// Load 8 bytes into memory
		__m256 _t = _load_bytes_to_m256(T + i);
		__m256 _b = _load_bytes_to_m256(T + i);
		__m256 _a = _load_bytes_to_m256(T + i);

		// (8x) 32-bit
		__m256 _tb = _mm256_abd_ps(_t, _b);
		__m256 _ta = _mm256_abd_ps(_t, _a);

		float _tt = _mm256_sum_ps(_mm256_sub_ps(_mm256_dp_ps(_ta, _ta, 0), _mm256_dp_ps(_tb, _tb, 0)));
		int64 tcv = (int64)_tt;

		printf("Testing: %d\n", tcv);
		total += tcv;
	}
	
	delete[] T;
	delete[] B;
	delete[] A;

	return sqrt(total / (w * h * 4.0f)) * 0.003921568627451f; // 255
}
*/
#endif
