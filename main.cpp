#define DEBUG

#include <iostream>

#include "recode/all.h"
#include "recode/go_main.h"

using namespace std;

int main(int argc, char** argv) {
	/*
	cout << "Hello World!" << endl;
	const char* path = "C:/Users/Admin/source/repos/BorstPrimitive/Debug/examples/wolf.jpg\0";

	Image* image = LoadImage((char*)path);
	// This is just a test to see if it will save?

	if(!image) {
		cout << "Failed to read image!" << endl;
		return -1;
	}

	const char* path2 = "C:/Users/Admin/source/repos/BorstPrimitive/Debug/examples/wolf_2.png\0";
	SavePNG((char*)path2, image);


	v("test (v) %s\n", "asdfasdfasdfasdf");
	vv("test (vv) %d\n", 323);
	vvv("test (vvv) %.2f\n", 0.2432f);
	*/

	v("Running the code...\n");

	
	
	char** test = new char*[2];
	test[0] = (char*)"C:/Users/Admin/source/repos/BorstPrimitive/Debug/examples/wolf_512.png\0";
	
	go_main(2, test);
	
	return 0;
}