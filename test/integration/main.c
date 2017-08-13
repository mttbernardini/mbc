#include <stdio.h>
#include <time.h>

#define CTEST_MAIN
#define CTEST_COLOR_OK

#include <ctest.h>

int main(int argc, const char* argv[]) {
	srand(time(NULL));

	puts("Running integration tests:");

    return ctest_main(argc, argv);
}
