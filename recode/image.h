#pragma once

#include "bundle.h"

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