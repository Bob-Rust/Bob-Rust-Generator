// #include "exporting.h"

#include <iostream>
#include "recode/all.h"
#include "recode/go_main.h"

using namespace std;

int main(int argc, char** argv) {
	Settings set;
	set.ImagePath = (char*)"Debug/examples/wolf_512.png";
	set.CallbackShapes = 100;
	set.MaxShapes = 8000;
	set.OutputSize = 1;
	set.Background = Color{0, 0, 0, 0};
	set.Alpha = 2;
	
	return run_borst_generator(set);
}
