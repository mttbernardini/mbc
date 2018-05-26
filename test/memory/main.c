#include <stdio.h>

#define CTEST_MAIN
#define CTEST_COLOR_OK

#include <ctest.h>

int main(int argc, const char* argv[]) {
	puts("Running memory allocation tests:");

	return ctest_main(argc, argv);
}
