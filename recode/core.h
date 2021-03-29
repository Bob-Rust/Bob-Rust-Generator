#include "all.h"
#include <math.h>

#ifndef __CORE_H__
#define __CORE_H__

#include "../utils.h"
#include "bundle.h"

typedef long long int64;
typedef unsigned long long uint64;

/*
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
*/

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
		/*
		// TODO: Use memcpy
		for(int x = line.x1; x < line.x2; x++) {
			dst->Pix[idx] = src->Pix[idx];
			idx++;
		}
		*/
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

// TODO: Use insintric functions to speedup this method. (32x) using AVX2
// [Only used once]
float differenceFull(Image* a, Image* b) {
	int w = a->width;
	int h = a->height;

	uint64 total = 0;
	Color* aaa = a->Pix;
	Color* bba = b->Pix;

	Color aa, bb;
	for(int y = 0; y < h; y++) {
		//int idx = y * w; // a->PixOffset(0, y);

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

// TODO: Use insintric functions to speedup this method.
//       We could save 20% of the time maybe.
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

// 32 bit
static inline __m256i _mm256_abd_epu8(__m256i a, __m256i b) {
    return _mm256_sub_epi8(_mm256_max_epu8(a, b), _mm256_min_epu8(a, b));
}

static inline int _hsum_16x16(__m128i v) {
	// (16x) 16-bit values
	// __m256i a = _mm256_hadd_epi16(v, v);// [ 0 0 0 0   1 1 1 1   2 2 2 2   3 3 3 3 ]
	
	// (8x) 16-bit  values
	/*__m128i aa = _mm_add_epi16(
		_mm256_castsi256_si128(a),		// [ 0 0 0 0   1 1 1 1 ]
		_mm256_extracti128_si256(a, 1)	// [ 2 2 2 2   3 3 3 3 ]
	);									// [ a b c d   e f g h ]
	

	// Is this good?
	__m128i a = _mm_hadd_epi16(v, v);// [ a+b c+d e+f g+h .... ]
	a = _mm_hadd_epi16(a, a);// [ abcd efgh ...... ]
	a = _mm_hadd_epi16(a, a);// [ abcdefgh ...... ]
	return _mm_extract_epi16(a, 0);
}

// Using AVX?
float differencePartial(Image* target, Image* before, Image* after, float score, vector<Scanline>& lines) {
	int w = target->width;
	int h = target->height;

	// TODO: Calculate the total maximum value that total could hold and maybe
	//       reduce the size from int64 to int
	int64 total = (int64)(pow(score * 255, 2) * (w * h * 4.0f));
	int64 told = total;
	// Note that the largest a scanline could be with the current setup is 30 pixels wide
	// Could this really be optimized?
	// Images must be atleast 32 * 32 in size

	int len = 0;
	for(unsigned int i = 0; i < lines.size(); i++) {
		Scanline line = lines[i];
		len += line.x2 - line.x1;
	}

	int tbl = (len + 31) & (~31);
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

	// The int lists are now filled with all the data that is needed and is a multiple of 32
	int stg = tbl >> 5;

	
	for(int i = 0; i < tbl; i += 32) {
		T[0] = 0x00000104;
		__m256i _t = *(__m256i*)(T + i);
		__m256i _b = *(__m256i*)(B + i);
		__m256i _a = *(__m256i*)(A + i);
		// (32x)

		__m256i _tb = _mm256_abd_epu8(_t, _b);
		__m256i _ta = _mm256_abd_epu8(_t, _a);
		// (32x)

		v("Aa: %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d | %d, %d, %d, %d\n",
			_mm256_extract_epi8(_tb, 0),
			_mm256_extract_epi8(_tb, 1),
			_mm256_extract_epi8(_tb, 2),
			_mm256_extract_epi8(_tb, 3),
			_mm256_extract_epi8(_tb, 4),
			_mm256_extract_epi8(_tb, 5),
			_mm256_extract_epi8(_tb, 6),
			_mm256_extract_epi8(_tb, 7),
			_mm256_extract_epi8(_tb, 8),
			_mm256_extract_epi8(_tb, 9),
			_mm256_extract_epi8(_tb, 10),
			_mm256_extract_epi8(_tb, 11),
			_mm256_extract_epi8(_tb, 12),
			_mm256_extract_epi8(_tb, 13),
			_mm256_extract_epi8(_tb, 14),
			_mm256_extract_epi8(_tb, 15),
			
			_mm256_extract_epi8(_tb, 16),
			_mm256_extract_epi8(_tb, 17),
			_mm256_extract_epi8(_tb, 18),
			_mm256_extract_epi8(_tb, 19)
		);

		// (8x) 16-bit values
		__m256i _tbb = _mm256_maddubs_epi16(_tb, _tb);
		__m256i _taa = _mm256_maddubs_epi16(_ta, _ta);
		// 16 values of 16-bit numbers

		v("A: %s\n", &_tbb);
		v("Ex: %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d\n",
			_mm256_extract_epi16(_tbb, 0),
			_mm256_extract_epi16(_tbb, 1),
			_mm256_extract_epi16(_tbb, 2),
			_mm256_extract_epi16(_tbb, 3),
			_mm256_extract_epi16(_tbb, 4),
			_mm256_extract_epi16(_tbb, 5),
			_mm256_extract_epi16(_tbb, 6),
			_mm256_extract_epi16(_tbb, 7),
			_mm256_extract_epi16(_tbb, 8),
			_mm256_extract_epi16(_tbb, 9),
			_mm256_extract_epi16(_tbb, 10),
			_mm256_extract_epi16(_tbb, 11),
			_mm256_extract_epi16(_tbb, 12),
			_mm256_extract_epi16(_tbb, 13),
			_mm256_extract_epi16(_tbb, 14),
			_mm256_extract_epi16(_tbb, 15)
		);

		int av = 0;//_hsum_16x16(_taa);
		int bv = 0;//_hsum_16x16(_tbb);
		total += av - bv;
		v("NEW: %d, %d, %d, total: %d\n", i + 32, av, bv, total);

		// 4 values of 32 bit numbers
		//  Perform _taa - _tbb
	}
	
	v("============================================\n");
	total = told;

	int ASD = 0;
	int tsc = 0;
	int64 accA = 0;
	int64 accB = 0;
	Color tt, bb, aa;
	for(unsigned int i = 0; i < lines.size(); i++) {
		Scanline line = lines[i];
		int idx = line.x1 + line.y * w;

		Color* tta = (target->Pix + idx - line.x1);
		Color* bba = (before->Pix + idx - line.x1);
		Color* aaa = (after->Pix + idx - line.x1);

		for(int x = line.x1; x <= line.x2; x++) {
			tt = tta[x];
			bb = bba[x];
			aa = aaa[x];
			
			// 8 of them

			
			int dr1 = (int)tt.r - (int)bb.r;
			int dg1 = (int)tt.g - (int)bb.g;
			int db1 = (int)tt.b - (int)bb.b;
			int da1 = (int)tt.a - (int)bb.a;
			int dr2 = (int)tt.r - (int)aa.r;
			int dg2 = (int)tt.g - (int)aa.g;
			int db2 = (int)tt.b - (int)aa.b;
			int da2 = (int)tt.a - (int)aa.a;
			
			
			int bv = int64(dr1*dr1 + dg1*dg1 + db1*db1 + da1*da1);
			int av = int64(dr2*dr2 + dg2*dg2 + db2*db2 + da2*da2);
			total += av - bv;
			accA += av;
			accB += bv;
			
			//v("A: %d, %d, %d, %d\n", tt.r, tt.g, tt.b, tt.a);
			//v("A: %d, %d, %d, %d\n", abs(dr1), abs(dg1), abs(db1), abs(da1));
			v("Ex: %d, %d, %d, %d\n", dr1*dr1, dg1*dg1, db1*db1, da1*da1);

			//printf("Check, %d, %d\n", T[ASD++], tta[x]);
			int TSC = tsc;
			if((tsc++ % 32) == 0) {
				v("OLD: %d, %d, %d, total: %d\n", TSC + 32, accA, accB, total);
				accA = 0;
				accB = 0;
			}
		}
	}

	v("--------------------------------------------\n");

	
	
	delete T;
	delete B;
	delete A;
	*
	return sqrt(total / (w * h * 4.0f)) * 0.003921568627451f; // 255
}
*/

#endif