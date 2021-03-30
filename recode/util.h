#pragma once

#include <fstream>
using std::ofstream;

#include "bundle.h"

#define STB_IMAGE_IMPLEMENTATION
//#define STBI_ONLY_PNG
#include "../stb/stb_image.h"

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "../stb/stb_image_write.h"

Image* LoadImage(char* path) {
	int w, h, n;
	unsigned char *data = stbi_load(path, &w, &h, &n, 0);
	if(!data) {
		stbi_image_free(data);
		return (Image*)0;
	}

	Image* image = new Image(w, h);
	switch(n) {
		case 1: {
			for(int i = 0; i < w * h; i++) {
				unsigned char c = data[i];
				image->Pix[i] = Color{ c, c, c, 0xff };
			}
			break;
		}
		case 2: {
			for(int i = 0; i < w * h; i++) {
				unsigned char c = data[i * 2];
				image->Pix[i] = Color{ c, c, c, data[i * 2 + 1] };
			}
			break;
		}
		case 3: {
			for(int i = 0; i < w * h; i++) {
				image->Pix[i] = Color{ data[i * 3], data[i * 3 + 1], data[i * 3 + 2], 0xff };
			}
			break;
		}
		case 4: {
			for(int i = 0; i < w * h; i++) {
				image->Pix[i] = Color{ data[i * 4], data[i * 4 + 1], data[i * 4 + 2], data[i * 4 + 3] };
			}
			break;
		}
	}
	
	stbi_image_free(data);
	return image;
}

// Returns false when not successfull
int SaveFile(char* path, char* contents) {
	ofstream stream;
	stream.open(path);

	if(!stream) {
		printf("Failed to open input stream for path '%s'\n", path);
		return 0;
	}

	stream << contents;

	return 1;
}

int SavePNG(char* path, Image* im) {
	/*int w = im->width;
	int h = im->height;

	unsigned char* pixel_array = new unsigned char[w * h * 4];
	for(int i = 0; i < w * h; i++) {
		Color c = im->Pix[i];
		pixel_array[i * 4    ] = c.r;
		pixel_array[i * 4 + 1] = c.g;
		pixel_array[i * 4 + 2] = c.b;
		pixel_array[i * 4 + 3] = c.r;
	}*/

	return stbi_write_png(path, im->width, im->height, 4, (void*)im->Pix, 0);
	//delete[] pixel_array;
	//return result;
}
/*
func SavePNG(path string, im image.Image) error {
	file, err := os.Create(path)
	if err != nil {
		return err
	}
	defer file.Close()
	return png.Encode(file, im)
}
*/

// Should not really do anything different
Image* imageToRGBA(Image* src) {
	return (Image*)src;
}
/*
func imageToRGBA(src image.Image) *image.RGBA {
	dst := image.NewRGBA(src.Bounds())
	draw.Draw(dst, dst.Rect, src, image.ZP, draw.Src)
	return dst
}
*/

// To avoid memory leaks call delete <copy> when done
Image* copyRGBA(Image* src) {
	Image* copy = new Image(src->width, src->height);

	// TODO: Use memcpy for this operation!!!
	int len = src->width * src->height;
	for(int i = 0; i < len; i++) {
		copy->Pix[i] = src->Pix[i];
	}

	return copy;
}

// [Used once]
Image* uniformRGBA(void* Rectangle, Color c) {
	return (Image*)0;
}
/*
func uniformRGBA(r image.Rectangle, c color.Color) *image.RGBA {
	im := image.NewRGBA(r)
	draw.Draw(im, im.Bounds(), &image.Uniform{c}, image.ZP, draw.Src)
	return im
}
*/