#pragma once

#ifndef BUILD_NODE
constexpr int BORST_INVALID_IMAGE	= 0x1000;
constexpr int BORST_ERROR			= 0x1001;
constexpr int BORST_SUCCESSFUL		= 0;

#include "util.h"
#include "bundle.h"
#include "model.h"
#include "../sort/blob_sort.h"

#include <chrono>
using std::chrono::high_resolution_clock;
using namespace std::chrono;

int run_borst_generator(Settings settings) {
	char* input = settings.ImagePath;

	printf("Reading from the file '%s'\n", input);

	Image* image = BorstLoadImage(input, settings.Width, settings.Height);
	if(!image) {
		printf("Failed to read image '%s'\n", input);
		return BORST_INVALID_IMAGE;
	}
	
	const int Count = settings.MaxShapes;
	const int Callb = settings.CallbackShapes;
	const int Alpha = ARR_ALPHAS[settings.Alpha];
	Model* model = new Model(image, settings.Background, Alpha);
	
	{
		Color bg = settings.Background;
		printf("Settings:\n");
		printf("  ImagePath  = %s\n", settings.ImagePath);
		printf("  MaxShapes  = %d\n", settings.MaxShapes);
		printf("  Callback   = %d\n", settings.CallbackShapes);
		printf("  Alpha      = %d\n", settings.Alpha);
		printf("  Dimensions = (%dx%d)\n", settings.Width, settings.Height);
		printf("  Background = (%d, %d, %d, %d)\n", bg.r, bg.g, bg.b, bg.a);
		printf("\n");
	}

	auto begin = high_resolution_clock::now();
	for(int i = 0; i <= Count; i++) {
		auto start = high_resolution_clock::now();
		int n = model->Step();
		
		if((i % Callb) == 0 || (i == Count)) {
			auto end = high_resolution_clock::now();
			auto elapsed = duration_cast<milliseconds>(end - begin);
			float time = elapsed.count() / 1000.0f;
			float sps = i / time;
			if(!isfinite(sps)) sps = 0;

			if(i == Count) {
				// Sort the shapes
				bobrust::sort_blob_list(model);
			}

			char stream_path[256];
			sprintf_s(stream_path, "Debug/outfolder/image_%d.png", i);
			
			printf("%5d: t=%.3f s, score=%.6f, n=%d, s/s=%.2f : '%s'\n", i, time, model->score, n, sps, stream_path);
			SavePNG(stream_path, &model->current[0]);
		}
	}

	delete model;
	delete image;

	return BORST_SUCCESSFUL;
}
#endif
