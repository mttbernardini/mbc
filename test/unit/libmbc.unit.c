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
