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

	Image* image = LoadImage(input);
	if(!image) {
		printf("Failed to read image '%s'\n", input);
		return BORST_INVALID_IMAGE;
	}

	Model* model = new Model(image, settings.Background, settings.OutputSize);
	const int Count = settings.MaxShapes;
	const int Callb = settings.CallbackShapes;
	const int Alpha = ARR_ALPHAS[settings.Alpha];
	int Repeat = 1;

	printf("Settings:\n");
	printf("  MaxShapes  = %d\n", Count);
	printf("  Callback   = %d\n", Callb);
	printf("  Alpha      = %d\n", Alpha);
	printf("\n");
	
	auto begin = high_resolution_clock::now();
	for(int i = 0; i <= Count; i++) {
		auto start = high_resolution_clock::now();
		int n = model->Step(Alpha, Repeat);
		auto end = high_resolution_clock::now();

		if((i % Callb) == 0) {
			auto elapsed = duration_cast<milliseconds>(end - start); 
			float nps = i / (duration_cast<seconds>(end - begin).count() + 0.0f); 
			if(!isfinite(nps)) nps = 0;

			std::stringstream stream;
			stream << "Debug/outfolder/image" << "_" << i << ".png";
			std::string str(stream.str());
			char* stream_path = (char*)str.c_str();

			printf("%5d: t=%.3f ms, score=%.6f, n=%d ms, n/s=%.2f : '%s'\n", i, elapsed.count() / 1000.0, model->score, n, nps, stream_path);
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

	Image* image = LoadImage(input);
	if(!image) return BORST_INVALID_IMAGE;

	Model* model = new Model(image, settings.Background, settings.OutputSize);
	const int Count = settings.MaxShapes;
	const int Callb = settings.CallbackShapes;
	const int Alpha = ARR_ALPHAS[settings.Alpha];
	int Repeat = 1;

	for(int i = 0; i <= Count; i++) {
		int n = model->Step(Alpha, Repeat);

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
