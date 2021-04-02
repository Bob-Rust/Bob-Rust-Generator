#ifndef __CIRCLE_CACHE_H__
#define __CIRCLE_CACHE_H__

#include "bundle.h"

const Scanline CIRCLE_0[1] {
	{0, 0, 0}
};

const Scanline CIRCLE_1[3] {
	{-1, -1, 1},
	{0, -1, 1},
	{1, -1, 1}
};

const Scanline CIRCLE_2[7] {
	{-3, -1, 1},
	{-2, -2, 2},
	{-1, -3, 3},
	{0, -3, 3},
	{1, -3, 3},
	{2, -2, 2},
	{3, -1, 1}
};

const Scanline CIRCLE_3[11] {
	{-5, -2, 2},
	{-4, -3, 3},
	{-3, -4, 4},
	{-2, -5, 5},
	{-1, -5, 5},
	{0, -5, 5},
	{1, -5, 5},
	{2, -5, 5},
	{3, -4, 4},
	{4, -3, 3},
	{5, -2, 2}
};

const Scanline CIRCLE_4[19] {
	{-9, -3, 3},
	{-8, -5, 5},
	{-7, -6, 6},
	{-6, -7, 7},
	{-5, -8, 8},
	{-4, -8, 8},
	{-3, -9, 9},
	{-2, -9, 9},
	{-1, -9, 9},
	{0, -9, 9},
	{1, -9, 9},
	{2, -9, 9},
	{3, -9, 9},
	{4, -8, 8},
	{5, -8, 8},
	{6, -7, 7},
	{7, -6, 6},
	{8, -5, 5},
	{9, -3, 3}
};

const Scanline CIRCLE_5[25] {
	{-12, -3, 3},
	{-11, -5, 5},
	{-10, -7, 7},
	{-9, -8, 8},
	{-8, -9, 9},
	{-7, -10, 10},
	{-6, -10, 10},
	{-5, -11, 11},
	{-4, -11, 11},
	{-3, -12, 12},
	{-2, -12, 12},
	{-1, -12, 12},
	{0, -12, 12},
	{1, -12, 12},
	{2, -12, 12},
	{3, -12, 12},
	{4, -11, 11},
	{5, -11, 11},
	{6, -10, 10},
	{7, -10, 10},
	{8, -9, 9},
	{9, -8, 8},
	{10, -7, 7},
	{11, -5, 5},
	{12, -3, 3}
};

const int CIRCLE_CACHE_LENGTH[6] { 1, 3, 7, 11, 19, 25 };
const Scanline* CIRCLE_CACHE[6] { CIRCLE_0, CIRCLE_1, CIRCLE_2, CIRCLE_3, CIRCLE_4, CIRCLE_5 };

#endif