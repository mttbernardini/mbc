#include <libmbc.c>
#include <ctest.h>

CTEST(libmbc, mbc_set_user_key__no_complement) {
	ASSERT_TRUE(mbc_set_user_key((uint8_t[]){0x22}, 1));
	ASSERT_NOT_NULL(user_key);
	ASSERT_NOT_NULL(oct_key);
	ASSERT_DATA((uint8_t[]){0x22}, 1, user_key, user_key_size);
	ASSERT_DATA((uint8_t[]){0x06}, 1, oct_key, oct_key_size); // 00100010 → 010 001 = swap 2 with 1 → 0000 0110 → 0x06
	mbc_free();
}

CTEST(libmbc, mbc_set_user_key__complement) {
	ASSERT_TRUE(mbc_set_user_key((uint8_t[]){0x2d}, 1));
	ASSERT_NOT_NULL(user_key);
	ASSERT_NOT_NULL(oct_key);
	ASSERT_DATA((uint8_t[]){0x2d}, 1, user_key, user_key_size);
	ASSERT_DATA((uint8_t[]){0x22}, 1, oct_key, oct_key_size); // 00101101 → 101 001 = swap 5 with 1 → 0010 0010 → 0x22
	mbc_free();
}

CTEST(libmbc, mbc_set_user_key__complement_indifferent) {
	ASSERT_TRUE(mbc_set_user_key((uint8_t[]){0x47}, 1));
	ASSERT_NOT_NULL(user_key);
	ASSERT_NOT_NULL(oct_key);
	ASSERT_DATA((uint8_t[]){0x47}, 1, user_key, user_key_size);
	ASSERT_DATA((uint8_t[]){0x18}, 1, oct_key, oct_key_size); // 01000111 → 100 011 = swap 4 with 3 → 0001 1000 → 0x18
	mbc_free();
}

CTEST(libmbc, mbc_set_user_key__empty) {
	mbc_set_user_key((uint8_t[]){}, 0);
	ASSERT_EQUAL(0, user_key_size);
	ASSERT_EQUAL(0, oct_key_size);
	mbc_free();
}

CTEST(libmbc, mbc_free) {
	mbc_free();
	ASSERT_EQUAL(0, user_key_size);
	ASSERT_EQUAL(0, oct_key_size);
	ASSERT_NULL(user_key);
	ASSERT_NULL(oct_key);
}


CTEST(libmbc, mbc_raw_to_hex__uppercase) {
	char* hex = mbc_raw_to_hex((uint8_t[]){0x00, 0x0a, 0x61}, 3, true);
	ASSERT_NOT_NULL(hex);
	ASSERT_STR("000A61", hex);
	free(hex);
}

CTEST(libmbc, mbc_raw_to_hex__lowercase) {
	char* hex = mbc_raw_to_hex((uint8_t[]){0x00, 0x0a, 0x61}, 3, false);
	ASSERT_NOT_NULL(hex);
	ASSERT_STR("000a61", hex);
	free(hex);
}

CTEST(libmbc, mbc_hex_to_raw) {
	size_t n;
	uint8_t* raw = mbc_hex_to_raw("000a61B2f", &n);
	ASSERT_NOT_NULL(raw);
	ASSERT_DATA(((uint8_t[]){0x00, 0x0a, 0x61, 0xb2}), 4, raw, n);
	free(raw);
}
