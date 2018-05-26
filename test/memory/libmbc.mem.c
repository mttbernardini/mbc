#include "../mem_control.h"
#include <libmbc.c>
#include <ctest.h>

#include <stdio.h>

CTEST(libmbc, mbc_set_user_key_fail) {
	MEM_CONTROL_FAIL_NEXT = true;
	ASSERT_FALSE(mbc_set_user_key((uint8_t*)"fail", 4));
}

CTEST(libmbc, mbc_encode_fail) {
	MEM_CONTROL_FAIL_NEXT = true;
	ASSERT_NULL(mbc_encode((uint8_t*)"fail", 4));
}

CTEST(libmbc, mbc_decode_fail) {
	MEM_CONTROL_FAIL_NEXT = true;
	ASSERT_NULL(mbc_decode((uint8_t*)"fail", 4));
}

CTEST(libmbc, mbc_encode_to_hex_fail) {
	MEM_CONTROL_FAIL_NEXT = true;
	ASSERT_NULL(mbc_encode_to_hex((uint8_t*)"fail", 4, false));
}

CTEST(libmbc, mbc_decode_from_hex_fail) {
	size_t decoded_size;

	MEM_CONTROL_FAIL_NEXT = true;
	ASSERT_NULL(mbc_decode_from_hex("fail", &decoded_size));
}

CTEST(libmbc, mbc_raw_to_hex_fail) {
	MEM_CONTROL_FAIL_NEXT = true;
	ASSERT_NULL(mbc_raw_to_hex((uint8_t*)"fail", 4, false));
}

CTEST(libmbc, mbc_hex_to_raw_fail) {
	size_t raw_size;

	MEM_CONTROL_FAIL_NEXT = true;
	ASSERT_NULL(mbc_hex_to_raw("fail", &raw_size));
}
