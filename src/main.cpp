#define BUILD_NODE

#include <iostream>
#include "recode/all.h"
#include "recode/go_main.h"
#include "exporting.h"

#ifndef BUILD_NODE
using namespace std;

int main(int argc, char** argv) {
	Settings set;
	set.ImagePath = (char*)"Debug/examples/wolf_512.png";
	set.CallbackShapes = 100;
	set.MaxShapes = 8000;
	set.Background = Color{0, 0, 0, 0xff};
	set.Alpha = 2;
	
	return run_borst_generator(set);
}
#endif
