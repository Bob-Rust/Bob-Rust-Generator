// This file contains the common types that is used in the program

#ifndef __BUNDLE_H__
#define __BUNDLE_H__

typedef union Color {
	struct {
		unsigned char r, g, b, a;
	};
	unsigned int rgba;
};

struct Scanline {
	int y, x1, x2;
};

#endif