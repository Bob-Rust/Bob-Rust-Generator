// This file contains the common types that is used in the program

#ifndef __BUNDLE_H__
#define __BUNDLE_H__

struct Color {
	unsigned char r, g, b, a;
};

struct Scanline {
	int y, x1, x2;
};

class Image {
	public:
		unsigned int width;
		unsigned int height;
		Color* Pix;

		Image(unsigned int width, unsigned int height) {
			this->width = width;
			this->height = height;
			this->Pix = new Color[width * height];
		}

		~Image() {
			delete[] Pix;
		}
};

#endif
