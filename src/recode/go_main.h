#pragma once

constexpr int BORST_INVALID_IMAGE	= 0x1000;
constexpr int BORST_ERROR			= 0x1001;
constexpr int BORST_SUCCESSFUL		= 0;

#include "util.h"
#include "bundle.h"
#include "model.h"

#ifndef BUILD_NODE

#include <chrono>
using std::chrono::high_resolution_clock;
using namespace std::chrono;

int run_borst_generator(Settings settings) {
	char* input = settings.ImagePath;

	printf("Reading from the file '%s'\n", input);

	Image* image = BorstLoadImage(input);
	if(!image) {
		printf("Failed to read image '%s'\n", input);
		return BORST_INVALID_IMAGE;
	}

	Model* model = new Model(image, settings.Background);
	const int Count = settings.MaxShapes;
	const int Callb = settings.CallbackShapes;
	const int Alpha = ARR_ALPHAS[settings.Alpha];
	
	printf("Settings:\n");
	printf("  MaxShapes  = %d\n", Count);
	printf("  Callback   = %d\n", Callb);
	printf("  Alpha      = %d\n", Alpha);
	printf("\n");
	
	auto begin = high_resolution_clock::now();
	for(int i = 0; i <= Count; i++) {
		auto start = high_resolution_clock::now();
		int n = model->Step(Alpha);
		
		if((i % Callb) == 0 || (i == Count)) {
			auto end = high_resolution_clock::now();
			auto elapsed = duration_cast<milliseconds>(end - begin);
			float time = elapsed.count() / 1000.0f;
			float sps = i / time;
			if(!isfinite(sps)) sps = 0;

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
#else
int run_borst_generator(Settings settings) {
	char* input = settings.ImagePath;

	Image* image = BorstLoadImage(input);
	if(!image) return BORST_INVALID_IMAGE;

	Model* model = new Model(image, settings.Background);
	const int Count = settings.MaxShapes;
	const int Callb = settings.CallbackShapes;
	const int Alpha = ARR_ALPHAS[settings.Alpha];
	
	for(int i = 0; i <= Count; i++) {
		int n = model->Step(Alpha);

		if((i % Callb) == 0) {
			// TODO: Call the node javascript callback!
		}
	}

	// TODO: Call the node javascript callback!

	delete model;
	delete image;

	return BORST_SUCCESSFUL;
}
#endif
