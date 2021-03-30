#pragma once

constexpr int GO_MAIN_SUCCESSFULL = 1;
constexpr int GO_MAIN_ERROR = 0;

#include <chrono>
using std::chrono::high_resolution_clock;
using namespace std::chrono;

#include "util.h"
#include "bundle.h"
#include "model.h"

int go_main(int argc, char** argv) {
	char* input = argv[0]; // First argument is the input file

	printf("Reading from the file '%s'\n", input);

	Image* image = LoadImage(input);
	if(!image) {
		printf("Failed to read image '%s'\n", input);
		return GO_MAIN_ERROR;
	}

	int OutputSize = 1; // Get this from the commandline
	int numWorkers = 7; // Get this from the commandline
	Color bg{0,0,0,0xff};//Get this from the commandline 
	

	// Create the model
	Model* model = new Model(image, bg, OutputSize, numWorkers);
	int Count = 20000;
	int Alpha = 72;
	int Repeat = 1;
	printf("count=%d, alpha=%d, repeat=%d\n", Count, Alpha, Repeat);
	
	auto begin = high_resolution_clock::now();
	int frame = 0;
	for(int i = 0; i <= Count; i++) {
		auto start = high_resolution_clock::now();
		int n = model->Step(Alpha, Repeat);
		auto end = high_resolution_clock::now();
		
		if((i % 100) == 0) {
			auto elapsed = duration_cast<milliseconds>(end - start); 
			float nps = i / (duration_cast<seconds>(end - begin).count() + 0.0f); 
			if(!isfinite(nps)) nps = 0;
			
			std::stringstream stream;
			stream << "Debug/outfolder/image" << "_" << i << ".png";
			
			std::string str(stream.str());
			char* stream_path = (char*)str.c_str();

			//printf("Save model to: '%s'\n", stream_path);
			printf("%5d: t=%.3f ms, score=%.6f, n=%d ms, n/s=%.2f : '%s'\n", frame, elapsed.count() / 1000.0, model->score, n, nps, stream_path);

			SavePNG(stream_path, &model->current[0]);
		}

		frame++;
	}

	return GO_MAIN_SUCCESSFULL;
}
