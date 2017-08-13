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


CTEST(libmbc, mbc_encode_inplace__null_key) {
	uint8_t data[] = {0x5d};
	ASSERT_TRUE(mbc_set_user_key((uint8_t[]){0x00}, 1));
	mbc_encode_inplace(data, 1);
	ASSERT_DATA((uint8_t[]){0x5d}, 1, data, 1);
	mbc_free();
}

CTEST(libmbc, mbc_decode_inplace__null_key) {
	uint8_t data[] = {0x5d};
	ASSERT_TRUE(mbc_set_user_key((uint8_t[]){0x00}, 1));
	mbc_decode_inplace(data, 1);
	ASSERT_DATA((uint8_t[]){0x5d}, 1, data, 1);
	mbc_free();
}

CTEST(libmbc, mbc_encode__matching_sizes) {
	ASSERT_TRUE(mbc_set_user_key((uint8_t[]){0x2d}, 1));
	// swap: 1-5
	// xor:  0x2d
	uint8_t* encoded = mbc_encode((uint8_t[]){0x12}, 1);  // swap 0001 0010 → 0011 0000 ^ 0010 1101 → 0001 1101
	ASSERT_DATA((uint8_t[]){0x1d}, 1, encoded, 1);
	free(encoded);
	mbc_free();
}

CTEST(libmbc, mbc_decode__matching_sizes) {
	ASSERT_TRUE(mbc_set_user_key((uint8_t[]){0x2d}, 1));
	// xor:  0x2d
	// swap: 1-5
	uint8_t* decoded = mbc_decode((uint8_t[]){0x1d}, 1);  // 0001 1101 ^ 0010 1101 → swap 0011 0000 → 0001 0010
	ASSERT_DATA((uint8_t[]){0x12}, 1, decoded, 1);
	free(decoded);
	mbc_free();
}

CTEST(libmbc, mbc_encode__longer_key) {
	ASSERT_TRUE(mbc_set_user_key((uint8_t[]){0x2d, 0x47}, 2));
	// swap: 1-5  3-4
	// xor:  0x2d 0x47 → 0x6a
	uint8_t* encoded = mbc_encode((uint8_t[]){0x12}, 1);  // swap 0001 0010 → 0010 1000 ^ 0110 1010 → 0100 0010
	ASSERT_DATA((uint8_t[]){0x42}, 1, encoded, 1);
	free(encoded);
	mbc_free();
}

CTEST(libmbc, mbc_decode__longer_key) {
	ASSERT_TRUE(mbc_set_user_key((uint8_t[]){0x2d, 0x47}, 2));
	// xor:  0x2d 0x47 → 0x6a
	// swap: 1-5  3-4
	uint8_t* decoded = mbc_decode((uint8_t[]){0x42}, 1);  // 0100 0010 ^ 0110 1010 → swap 0010 1000 → 0001 0010
	ASSERT_DATA((uint8_t[]){0x12}, 1, decoded, 1);
	free(decoded);
	mbc_free();
}

CTEST(libmbc, mbc_encode__longer_data) {
	ASSERT_TRUE(mbc_set_user_key((uint8_t[]){0x47}, 1));
	// swap: 3-4
	// xor:  0x47
	uint8_t* encoded = mbc_encode((uint8_t[]){0x57, 0x5a}, 2);
	// swap 0101 0111 → 0100 1111 ^ 0100 0111 → 0000 1000
	// swap 0101 1010 → 0101 1010 ^ 0100 0111 → 0001 1101
	ASSERT_DATA(((uint8_t[]){0x08, 0x1d}), 2, encoded, 2);
	free(encoded);
	mbc_free();
}

CTEST(libmbc, mbc_decode__longer_data) {
	ASSERT_TRUE(mbc_set_user_key((uint8_t[]){0x47}, 1));
	// xor:  0x47
	// swap: 3-4
	uint8_t* decoded = mbc_decode((uint8_t[]){0x08, 0x1d}, 2);
	// 0000 1000 ^ 0100 0111 → swap 0100 1111 → 0101 0111
	// 0001 1101 ^ 0100 0111 → swap 0101 1010 → 0101 1010
	ASSERT_DATA(((uint8_t[]){0x57, 0x5a}), 2, decoded, 2);
	free(decoded);
	mbc_free();
}


CTEST(libmbc, mbc_encode_to_hex) {
	ASSERT_TRUE(mbc_set_user_key((uint8_t[]){0x47}, 1));
	// swap: 3-4
	// xor:  0x47
	char* encoded = mbc_encode_to_hex((uint8_t[]){0x57, 0x5a}, 2, false);
	// swap 0101 0111 → 0100 1111 ^ 0100 0111 → 0000 1000
	// swap 0101 1010 → 0101 1010 ^ 0100 0111 → 0001 1101
	ASSERT_STR("081d", encoded);
	free(encoded);
	mbc_free();
}

CTEST(libmbc, mbc_decode_from_hex) {
	size_t dec_size;
	ASSERT_TRUE(mbc_set_user_key((uint8_t[]){0x47}, 1));
	// xor:  0x47
	// swap: 3-4
	uint8_t* decoded = mbc_decode_from_hex("081d", &dec_size);
	// 0000 1000 ^ 0100 0111 → swap 0100 1111 → 0101 0111
	// 0001 1101 ^ 0100 0111 → swap 0101 1010 → 0101 1010
	ASSERT_DATA(((uint8_t[]){0x57, 0x5a}), 2, decoded, dec_size);
	free(decoded);
	mbc_free();
}
