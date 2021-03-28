#pragma once

constexpr int paintingToolScale = 1;

#ifndef __UTILS_H__
#define __UTILS_H__

#include "recode/bundle.h"


// Alphas
	constexpr int NUM_APLHAS = 6;
	const int ARR_ALPHAS[NUM_APLHAS] = { 9, 18, 72, 136, 218, 255 };
// Sizes
	constexpr int NUM_SIZES = 6;
	const int ARR_SIZES[NUM_SIZES] = { 1, 2, 4, 6, 10, 13 };
	const int SIZES_TO_IDX[] { 0, 0, 1, 0, 2, 0, 3, 0, 0, 0, 4, 0, 0, 5 };
	#define SIZE_INDEX(size) SIZES_TO_IDX[size]

	#define COLOR(R,G,B,A) Color{(unsigned char)R, (unsigned char)G, (unsigned char)B, (unsigned char)A}
// Colors
	constexpr char COMMON_OPACITY = 72;
	constexpr int NUM_COLORS = 20;
	const Color ARR_COLORS[NUM_COLORS] = {
		COLOR(46, 204, 112, COMMON_OPACITY),
		COLOR(22, 161, 132, COMMON_OPACITY),
		COLOR(52, 153, 218, COMMON_OPACITY),
		COLOR(241, 195, 16, COMMON_OPACITY),
		COLOR(143, 69, 173, COMMON_OPACITY),
		COLOR(153, 163, 162, COMMON_OPACITY),
		COLOR(52, 73, 93, COMMON_OPACITY),
		COLOR(46, 158, 135, COMMON_OPACITY),
		COLOR(30, 224, 24, COMMON_OPACITY),
		COLOR(176, 122, 195, COMMON_OPACITY),
		COLOR(231, 127, 33, COMMON_OPACITY),
		COLOR(236, 240, 241, COMMON_OPACITY),
		COLOR(38, 174, 96, COMMON_OPACITY),
		COLOR(33, 203, 241, COMMON_OPACITY),
		COLOR(126, 77, 41, COMMON_OPACITY),
		COLOR(239, 68, 49, COMMON_OPACITY),
		COLOR(74, 212, 188, COMMON_OPACITY),
		COLOR(69, 48, 33, COMMON_OPACITY),
		COLOR(49, 49, 49, COMMON_OPACITY),
		COLOR(1, 2, 1, COMMON_OPACITY)
	};

// Utils
#define clampInt(a, b, c) (a < b ? b:(a > c ? c:a))
#define BORST_ABS(a, b) (a < b) ? (b - a):(a - b)
#define diffInt(a, b) (a < b) ? (b - a):(a - b)

#define COLOR_DISTANCE(a, b) (a.r - b.r) * (a.r - b.r) + (a.g - b.g) * (a.g - b.g) + (a.b - b.b) * (a.b - b.b)

Color closestColor(Color& color) {
	Color result = ARR_COLORS[0];
	unsigned int dist = 0xffffff;

	for(int i = 0; i < NUM_COLORS; i++) {
		Color b = ARR_COLORS[i];
		unsigned int d = COLOR_DISTANCE(color, b);

		if(d < dist) {
			dist = d;
			result = b;
		}
	}

	return result;
}

int closestColorIndex(Color& color) {
	int result = 0;
	unsigned int dist = 0;

	for(int i = 0; i < NUM_COLORS; i++) {
		Color b = ARR_COLORS[i];
		unsigned int d = COLOR_DISTANCE(color, b);

		if(i == 0 || d < dist) {
			dist = d;
			result = i;
		}
	}

	return result;
}

int closestSize(int size) {
	int closest = 0;
	int dist = 0xffff;
	for(int i = 0; i < NUM_SIZES; i++) {
		int d = BORST_ABS(size, ARR_SIZES[i]);

		if(d < dist) {
			dist = d;
			closest = i;
		}
	}

	return ARR_SIZES[closest];
}

// This can be made instant with lookups
// TODO: Create a 255 array that maps perfectly to the alpha values.
int closestAlpha(int alpha) {
	int closest = 0;
	int dist = 0xffff;
	for(int i = 0; i < NUM_SIZES; i++) {
		int d = BORST_ABS(alpha, ARR_ALPHAS[i]);

		if(d < dist) {
			dist = d;
			closest = i;
		}
	}

	return ARR_ALPHAS[closest];
}

#endif
