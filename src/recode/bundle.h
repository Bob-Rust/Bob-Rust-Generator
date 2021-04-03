// This file contains the common types that is used in the program

#ifndef __BUNDLE_H__
#define __BUNDLE_H__

struct Scanline { int y, x1, x2; };
struct Color { unsigned char r, g, b, a; };

struct Settings {
	// A path to the image that should be loaded
	char* ImagePath = 0;

	// How many shapes to generate before it sends
	// the data back to the caller.
	int CallbackShapes = 100;

	// The max amount of shapes to generate.
	int MaxShapes = 4000;

	// The default background color of the canvas.
	Color Background{0, 0, 0, 0xff};

	// The default alpha value.
	int Alpha = 2;

	// The width of the image
	int Width = 0;

	// The height of the image
	int Height = 0;
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

		Image(unsigned char* rgba, unsigned int width, unsigned int height, unsigned int target_width, unsigned int target_height) {
			this->width = target_width;
			this->height = target_height;
			this->Pix = new Color[target_width * target_height];

			for(int y = 0; y < target_height; y++) {
				int yp = y * target_width;
				int t_yp = ((y * height) / target_height) * width;
				for(int x = 0; x < target_width; x++) {
					int idx = x + yp;
					int t_idx = ((x * width) / target_width) + t_yp;
					
					this->Pix[idx] = ((Color*)rgba)[t_idx];
				}
			}
		}

		~Image() {
			delete[] Pix;
		}
};

#endif
