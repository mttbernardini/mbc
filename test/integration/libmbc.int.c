#include <time.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include <ctest.h>
#include <libmbc.h>

static const size_t MIN_KEY_SIZE = 32L;
static const size_t MAX_KEY_SIZE = 1024L * 1024L * 32L;
static const size_t MIN_RAW_DATA_SIZE = 128L;
static const size_t MAX_RAW_DATA_SIZE = 1024L * 1024L * 128L;
static const char HEX_CHARS_UPPER[] = {'0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'A', 'B', 'C', 'D', 'E', 'F'};
static const char HEX_CHARS_LOWER[] = {'0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'a', 'b', 'c', 'd', 'e', 'f'};

static const char HEXMAP_UPPER[256][2] = {
	"00", "01", "02", "03", "04", "05", "06", "07",	"08", "09", "0A", "0B", "0C", "0D", "0E", "0F",
	"10", "11", "12", "13", "14", "15", "16", "17",	"18", "19", "1A", "1B", "1C", "1D", "1E", "1F",
	"20", "21", "22", "23", "24", "25", "26", "27",	"28", "29", "2A", "2B", "2C", "2D", "2E", "2F",
	"30", "31", "32", "33", "34", "35", "36", "37",	"38", "39", "3A", "3B", "3C", "3D", "3E", "3F",
	"40", "41", "42", "43", "44", "45", "46", "47",	"48", "49", "4A", "4B", "4C", "4D", "4E", "4F",
	"50", "51", "52", "53", "54", "55", "56", "57",	"58", "59", "5A", "5B", "5C", "5D", "5E", "5F",
	"60", "61", "62", "63", "64", "65", "66", "67",	"68", "69", "6A", "6B", "6C", "6D", "6E", "6F",
	"70", "71", "72", "73", "74", "75", "76", "77",	"78", "79", "7A", "7B", "7C", "7D", "7E", "7F",
	"80", "81", "82", "83", "84", "85", "86", "87",	"88", "89", "8A", "8B", "8C", "8D", "8E", "8F",
	"90", "91", "92", "93", "94", "95", "96", "97",	"98", "99", "9A", "9B", "9C", "9D", "9E", "9F",
	"A0", "A1", "A2", "A3", "A4", "A5", "A6", "A7",	"A8", "A9", "AA", "AB", "AC", "AD", "AE", "AF",
	"B0", "B1", "B2", "B3", "B4", "B5", "B6", "B7",	"B8", "B9", "BA", "BB", "BC", "BD", "BE", "BF",
	"C0", "C1", "C2", "C3", "C4", "C5", "C6", "C7",	"C8", "C9", "CA", "CB", "CC", "CD", "CE", "CF",
	"D0", "D1", "D2", "D3", "D4", "D5", "D6", "D7",	"D8", "D9", "DA", "DB", "DC", "DD", "DE", "DF",
	"E0", "E1", "E2", "E3", "E4", "E5", "E6", "E7",	"E8", "E9", "EA", "EB", "EC", "ED", "EE", "EF",
	"F0", "F1", "F2", "F3", "F4", "F5", "F6", "F7",	"F8", "F9", "FA", "FB", "FC", "FD", "FE", "FF"
};

static const char HEXMAP_LOWER[256][2] = {
	"00", "01", "02", "03", "04", "05", "06", "07",	"08", "09", "0a", "0b", "0c", "0d", "0e", "0f",
	"10", "11", "12", "13", "14", "15", "16", "17",	"18", "19", "1a", "1b", "1c", "1d", "1e", "1f",
	"20", "21", "22", "23", "24", "25", "26", "27",	"28", "29", "2a", "2b", "2c", "2d", "2e", "2f",
	"30", "31", "32", "33", "34", "35", "36", "37",	"38", "39", "3a", "3b", "3c", "3d", "3e", "3f",
	"40", "41", "42", "43", "44", "45", "46", "47",	"48", "49", "4a", "4b", "4c", "4d", "4e", "4f",
	"50", "51", "52", "53", "54", "55", "56", "57",	"58", "59", "5a", "5b", "5c", "5d", "5e", "5f",
	"60", "61", "62", "63", "64", "65", "66", "67",	"68", "69", "6a", "6b", "6c", "6d", "6e", "6f",
	"70", "71", "72", "73", "74", "75", "76", "77",	"78", "79", "7a", "7b", "7c", "7d", "7e", "7f",
	"80", "81", "82", "83", "84", "85", "86", "87",	"88", "89", "8a", "8b", "8c", "8d", "8e", "8f",
	"90", "91", "92", "93", "94", "95", "96", "97",	"98", "99", "9a", "9b", "9c", "9d", "9e", "9f",
	"a0", "a1", "a2", "a3", "a4", "a5", "a6", "a7",	"a8", "a9", "aa", "ab", "ac", "ad", "ae", "af",
	"b0", "b1", "b2", "b3", "b4", "b5", "b6", "b7",	"b8", "b9", "ba", "bb", "bc", "bd", "be", "bf",
	"c0", "c1", "c2", "c3", "c4", "c5", "c6", "c7",	"c8", "c9", "ca", "cb", "cc", "cd", "ce", "cf",
	"d0", "d1", "d2", "d3", "d4", "d5", "d6", "d7",	"d8", "d9", "da", "db", "dc", "dd", "de", "df",
	"e0", "e1", "e2", "e3", "e4", "e5", "e6", "e7",	"e8", "e9", "ea", "eb", "ec", "ed", "ee", "ef",
	"f0", "f1", "f2", "f3", "f4", "f5", "f6", "f7",	"f8", "f9", "fa", "fb", "fc", "fd", "fe", "ff"
};

static const uint8_t HEXMAP_REVERSE[128] = {
	0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0,
	0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0,
	0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0,
	0x0, 0x1, 0x2, 0x3, 0x4, 0x5, 0x6, 0x7, 0x8, 0x9, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0,
	0x0, 0xA, 0xB, 0xC, 0xD, 0xE, 0xF, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0,
	0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0,
	0x0, 0xA, 0xB, 0xC, 0xD, 0xE, 0xF, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0,
	0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0,
};

static void random_test_data(uint8_t** key,      size_t* key_size,
                             void*     dummy,    size_t* dummy_size,
                             void*     expected, size_t* expected_size,
                             bool from_hex, bool to_hex, bool uppercase) {
	size_t i;

	if (*key_size == 0)
		*key_size = (size_t)(rand() % MAX_KEY_SIZE + MIN_KEY_SIZE + 1);

	if (*dummy_size == 0)
		*dummy_size = (size_t)(rand() % MAX_RAW_DATA_SIZE + MIN_RAW_DATA_SIZE + 1);

	if (*expected_size == 0) {
		if (from_hex && !to_hex)
			*expected_size = (*dummy_size)/2;
		else if (to_hex)
			*expected_size = (*dummy_size)*2;
		else
			*expected_size = *dummy_size;
	}

	CTEST_LOG("Key size..........: %zu bytes.", *key_size);
	CTEST_LOG("Dummy data size...: %zu bytes (%s).", *dummy_size, from_hex ? "hex" : "raw");
	CTEST_LOG("Expected data size: %zu bytes (%s).", *expected_size, to_hex ? "hex" : "raw");

	*key = malloc(*key_size);
	for (i = 0; i < *key_size; i++)
		(*key)[i] = (uint8_t)(rand() % 0xFF);

	if (from_hex) {
		*(char**)dummy = malloc(*dummy_size + 1);

		if (uppercase)
			for (i = 0; i < *dummy_size; i++)
				(*(char**)dummy)[i] = HEX_CHARS_UPPER[rand() % 0x10];
		else
			for (i = 0; i < *dummy_size; i++)
				(*(char**)dummy)[i] = HEX_CHARS_LOWER[rand() % 0x10];

		(*(char**)dummy)[*dummy_size] = '\0';

		if (to_hex) {
			*(char**)expected = malloc(*expected_size + 1);
			strcpy(*(char**)expected, *(char**)dummy);
		} else {
			*(uint8_t**)expected = malloc(*expected_size);
			for (i = 0; i < *dummy_size; i++)
				(*(uint8_t**)expected)[i] = HEXMAP_REVERSE[(uint8_t)(*(char**)dummy)[i*2]] * 0x10 + HEXMAP_REVERSE[(uint8_t)(*(char**)dummy)[i*2 + 1]];
		}
	} else {
		*(uint8_t**)dummy = malloc(*dummy_size);

		for (i = 0; i < *dummy_size; i++)
			(*(uint8_t**)dummy)[i] = (uint8_t)(rand() % 0xFF);

		if (to_hex) {
			*(char**)expected = malloc(*expected_size + 1);

			if (uppercase)
				for (i = 0; i < *dummy_size; i++)
					memcpy((*(char**)expected) + i*2, HEXMAP_UPPER[(*(uint8_t**)dummy)[i]], 2);
			else
				for (i = 0; i < *dummy_size; i++)
					memcpy((*(char**)expected) + i*2, HEXMAP_LOWER[(*(uint8_t**)dummy)[i]], 2);

			(*(char**)expected)[*expected_size] = '\0';
		} else {
			*(uint8_t**)expected = malloc(*expected_size);
			memcpy(*(uint8_t**)expected, *(uint8_t**)dummy, *expected_size);
		}
	}
}

CTEST(libmbc, encode_decode__inplace__raw_to_raw) {
	size_t ks, ds, es;
	uint8_t *k, *d, *e;

	ks = 0; ds = 0; es = 0;
	random_test_data(&k, &ks, &d, &ds, &e, &es, false, false, false);

	mbc_set_user_key(k, ks);
	mbc_encode_inplace(d, ds);
	mbc_decode_inplace(d, ds);
	ASSERT_DATA(e, es, d, ds);

	free(k);
	free(d);
	free(e);
	mbc_free();
}

CTEST(libmbc, encode_decode__inplace__raw_to_hex__lower) {
	size_t ks, ds, es;
	uint8_t *k, *d;
	char *dx, *e;

	ks = 0; ds = 0; es = 0;
	random_test_data(&k, &ks, &d, &ds, &e, &es, false, true, false);
	ASSERT_EQUAL(ds*2, es);

	mbc_set_user_key(k, ks);
	mbc_encode_inplace(d, ds);
	mbc_decode_inplace(d, ds);
	dx = mbc_raw_to_hex(d, ds, false);
	ASSERT_STR(e, dx);

	free(k);
	free(d);
	free(dx);
	free(e);
	mbc_free();
}
