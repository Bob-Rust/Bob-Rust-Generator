#pragma once

#include <fstream>
using std::ofstream;

#include <stdio.h>
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
	return stbi_write_png(path, im->width, im->height, 4, (void*)im->Pix, 0);
}

// Should not really do anything different
Image* imageToRGBA(Image* src) {
	return (Image*)src;
}

Image* copyRGBA(Image* src) {
	int w = src->width;
	int h = src->height;
	Image* dst = new Image(w, h);
	int len = src->width * src->height;
	memcpy(dst->Pix, src->Pix, w * h * sizeof(Color));
	return dst;
}
