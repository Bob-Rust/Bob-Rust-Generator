#define DEBUG

#include <iostream>

#include "recode/all.h"
#include "recode/go_main.h"

using namespace std;

int main(int argc, char** argv) {
	printf("Running the code...\n");

	#ifdef DEBUG
	{
		// Initialize some special values
		char* old = argv[0];
		
		// Super hacky
		argc = 1;
		argv = new char*[argc];
		argv[0] = (char*)"Debug/examples/wolf_512.png";
	}
	#endif
	
	printf("Args:\n");
	for(int i = 0; i < argc; i++) {
		printf(" [%d]: '%s'\n", i, argv[i]);
	}
	printf("\n");
	
	go_main(argc, argv);
	
	return 0;
}