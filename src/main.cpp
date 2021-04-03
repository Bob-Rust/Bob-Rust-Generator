// Used inside rand.h to seed the random generator with the current time
//#define _SEED_WITH_TIME

// Used to build a node module
#define BUILD_NODE

// Used to debug the sorting algorithm
//#define _DEBUG_BLOB_SORTER

#include "utils.h"
#include "recode/all.h"
#include "recode/go_main.h"
#include "exporting.h"

#ifndef BUILD_NODE
int main(int argc, char** argv) {
	
	Settings set;
	set.ImagePath = (char*)"Debug/examples/wolf_512.png";
	set.CallbackShapes = 100;
	set.MaxShapes = 4000;
	set.Background = Color{0, 0, 0, 0xff};
	set.Alpha = 2;
	
	return run_borst_generator(set);
}
#endif
