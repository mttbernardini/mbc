#include <stdio.h>
#include <stdlib.h>
#include <ctest.h>

CTEST(mbc, bash_script) {
	puts("");
	ASSERT_EQUAL(0, system("./mbc_random_test 512k 32k"));
}
