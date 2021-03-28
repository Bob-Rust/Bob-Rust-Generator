#pragma once

constexpr int GO_MAIN_SUCCESSFULL = 1;
constexpr int GO_MAIN_ERROR = 0;

#include <chrono>
using std::chrono::high_resolution_clock;
using namespace std::chrono;

#include "log.h"
#include "util.h"
#include "bundle.h"
#include "model.h"

int go_main(int argc, char** argv) {
	char* input = argv[0]; // First argument is the input file

	v("Reading from the file '%s'\n", input);

	Image* image = LoadImage(input);
	if(!image) {
		v("Failed to read image '%s'\n", input);
		return GO_MAIN_ERROR;
	}

	int OutputSize = 1; // Get this from the commandline
	int numWorkers = 7; // Get this from the commandline
	Color bg{0,0,0,0xff};//Get this from the commandline 
	

	// Start the algorithm
	Model* model = new Model(image, bg, OutputSize, numWorkers);
	int Count = 4000;
	ShapeType Mode = ShapeType::ShapeTypeCircle;
	int Alpha = 72;
	int Repeat = 1;
	v("count=%d, mode=%d, alpha=%d, repeat=%d\n", Count, Mode, Alpha, Repeat);
	

	auto begin = high_resolution_clock::now();
	int frame = 0;
	for(int i = 0; i < Count; i++) {
		frame++;

		// Find optimal shape and add it to the model
		auto start = high_resolution_clock::now();
		
		// t := time.Now()
		int n = model->Step(Mode, Alpha, Repeat);
		auto end = high_resolution_clock::now();
		auto elapsed = duration_cast<milliseconds>(end - start); 
		float nps = n / (duration_cast<milliseconds>(end - begin).count() + 0.0f); 


		if((i % 40) == 0) {
			v("%d: t=%.3f, score=%.6f, n=%d, n/s=%.2f\n", frame, elapsed.count() / 1000.0, model->score, n, nps);

			std:stringstream stream;
			stream << "C:/Users/Admin/source/repos/BorstPrimitive/Debug/outfolder/image" << "_" << i << ".png";
		
			std::string str(stream.str());
			char* stream_path = (char*)str.c_str();

			v("Save model to: '%s'\n", stream_path);
			SavePNG(stream_path, &model->current[0]);
		}
	}

	return GO_MAIN_SUCCESSFULL;
}