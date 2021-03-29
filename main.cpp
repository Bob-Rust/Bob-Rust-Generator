#define DEBUG

#include <iostream>

#include "recode/all.h"
#include "recode/go_main.h"

using namespace std;

int main(int argc, char** argv) {
	v("Running the code...\n");

	#ifdef DEBUG
	{
		// Initialize some special values
		char* old = argv[0];
		argv = new char*[2];

		// Hacky...
		argv[0] = (char*)"Debug/examples/wolf_512.png";
	}
	#endif
	
	v("Args:\n");
	for(int i = 0; i < argc; i++) {
		v(" [%d]: '%s'\n", i, argv[i]);
	}
	v("\n");
	
	go_main(2, argv);
	
	return 0;
}