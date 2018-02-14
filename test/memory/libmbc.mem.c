#include "../mfail_pre.h"
#include <libmbc.c>
#include "../mfail_post.h"

#include <ctest.h>

CTEST(libmbc, mbc_set_user_key_fail) {
	ASSERT_FALSE(mbc_set_user_key(NULL, 1));
}

CTEST(libmbc, mbc_encode_fail) {
	ASSERT_NULL(mbc_encode(NULL, 1));
}

CTEST(libmbc, mbc_decode_fail) {
	ASSERT_NULL(mbc_decode(NULL, 1));
}

CTEST(libmbc, mbc_encode_to_hex_fail) {
	ASSERT_NULL(mbc_encode_to_hex(NULL, 1, false));
}

CTEST(libmbc, mbc_decode_from_hex_fail) {
	ASSERT_NULL(mbc_decode_from_hex("fail", NULL));
}

CTEST(libmbc, mbc_raw_to_hex_fail) {
	ASSERT_NULL(mbc_raw_to_hex(NULL, 1, false));
}

CTEST(libmbc, mbc_hex_to_raw_fail) {
	ASSERT_NULL(mbc_hex_to_raw("fail", NULL));
}
