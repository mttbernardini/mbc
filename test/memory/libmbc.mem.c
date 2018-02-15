#include "../mem_control.h"
#include <libmbc.c>
#include <ctest.h>

#include <stdio.h>

CTEST(libmbc, mbc_set_user_key_fail) {
	MEM_CONTROL_FAIL_NEXT = true;
	ASSERT_FALSE(mbc_set_user_key(NULL, 1));
}

CTEST(libmbc, mbc_encode_fail) {
	MEM_CONTROL_FAIL_NEXT = true;
	ASSERT_NULL(mbc_encode(NULL, 1));
}

CTEST(libmbc, mbc_decode_fail) {
	MEM_CONTROL_FAIL_NEXT = true;
	ASSERT_NULL(mbc_decode(NULL, 1));
}

CTEST(libmbc, mbc_encode_to_hex_fail) {
	MEM_CONTROL_FAIL_NEXT = true;
	ASSERT_NULL(mbc_encode_to_hex(NULL, 1, false));
}

CTEST(libmbc, mbc_decode_from_hex_fail) {
	MEM_CONTROL_FAIL_NEXT = true;
	ASSERT_NULL(mbc_decode_from_hex("fail", NULL));
}

CTEST(libmbc, mbc_raw_to_hex_fail) {
	MEM_CONTROL_FAIL_NEXT = true;
	ASSERT_NULL(mbc_raw_to_hex(NULL, 1, false));
}

CTEST(libmbc, mbc_hex_to_raw_fail) {
	MEM_CONTROL_FAIL_NEXT = true;
	ASSERT_NULL(mbc_hex_to_raw("fail", NULL));
}
