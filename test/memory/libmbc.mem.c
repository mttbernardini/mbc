#include "../mfail_pre.h"
#include <libmbc.c>
#include "../mfail_post.h"

#include <ctest.h>


CTEST(libmbc, mbc_set_user_key_fail) {
	ASSERT_FALSE(mbc_set_user_key((uint8_t[]){0x10}, 1));
}
