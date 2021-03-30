// This file contains the common types that is used in the program

#ifndef __BUNDLE_H__
#define __BUNDLE_H__

typedef union Color {
	struct {
		unsigned char r, g, b, a;
	};
	//unsigned int rgba;
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

		int PixOffset(int x, int y) {
			return x + y * this->width;
		}
};

#endif