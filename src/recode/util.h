#pragma once

#include <stdio.h>
#include "bundle.h"

#define STB_IMAGE_IMPLEMENTATION
//#define STBI_ONLY_PNG
#include "../stb/stb_image.h"

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "../stb/stb_image_write.h"

unsigned char* BorstLoadImageDataRGBA(char* path, int& w, int& h) {
	int n;
	unsigned char *data = stbi_load(path, &w, &h, &n, 0);
	if(!data) {
		stbi_image_free(data);
		return 0;
	}

	Color *raw = new Color[w * h];
	
	switch(n) {
		case 1: {
			for(int i = 0; i < w * h; i++) {
				unsigned char c = data[i];
				raw[i] = { c, c, c, 0xff };
			}
			break;
		}
		case 2: {
			for(int i = 0; i < w * h; i++) {
				unsigned char c = data[i * 2];
				raw[i] = { c, c, c, data[i * 2 + 1] };
			}
			break;
		}
		case 3: {
			for(int i = 0; i < w * h; i++) {
				raw[i] = { data[i * 3], data[i * 3 + 1], data[i * 3 + 2], 0xff };
			}
			break;
		}
		case 4: {
			for(int i = 0; i < w * h; i++) {
				raw[i] = { data[i * 4], data[i * 4 + 1], data[i * 4 + 2], data[i * 4 + 3] };
			}
			break;
		}
	}
	
	stbi_image_free(data);
	return (unsigned char*)raw;
}

Image* BorstLoadImage(char* path, int target_width, int target_height) {
	int w, h;
	unsigned char* raw = BorstLoadImageDataRGBA(path, w, h);
	if(!raw) return 0;

	Image* result = new Image(raw, w, h, (!target_width) ? w:target_width, (!target_height) ? h:target_height);
	delete raw;
	return result;
}

Image* BorstLoadImage(char* path) {
	return BorstLoadImage(path, 0, 0);
}

int SavePNG(char* path, Image* im) {
	return stbi_write_png(path, im->width, im->height, 4, (void*)im->Pix, 0);
}

Image* copyRGBA(Image* src) {
	int w = src->width;
	int h = src->height;

	Image* dst = new Image(w, h);
	int len = w * h;
	memcpy(dst->Pix, src->Pix, len * sizeof(Color));
	return dst;
}
