#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <libmbc.h>

/***********************
 **      PRIVATE      **
 ***********************/

static uint8_t* xor_key;
static size_t   xor_key_size;
static uint8_t  swap_key[8];
static size_t   swap_key_size;

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

/**
 * Generates the octal key (to be used in swap phase) from a user key passed as parameter.
 * @post `okey` contains the swapping key;
 *       `*okey_size_ptr` contains the size of the actual generated key.
 */
static void make_swap_key(uint8_t okey[8], size_t* okey_size_ptr, const uint8_t* key, size_t key_size) {
	uint8_t l_bit, r_bit, dummy_temp, current, next;
	uint8_t swap_map[8], dummy_byte[8] = {0,1,2,3,4,5,6,7};
	bool to_check[8] = {1,1,1,1,1,1,1,1};
	register size_t i;
	size_t okey_size = 0;

	for (i = 0; i < key_size; i++) {
		l_bit = (key[i] >> 4) & 0x07;
		r_bit = (key[i] >> 1) & 0x07;

		if (l_bit != r_bit) {
			if ((key[i] & 0x01) && l_bit != (r_bit ^ 0x07)) {
				l_bit ^= 0x07;
				r_bit ^= 0x07;
			}

			dummy_temp        = dummy_byte[l_bit];
			dummy_byte[l_bit] = dummy_byte[r_bit];
			dummy_byte[r_bit] = dummy_temp;
		}
	}

	for (i = 0; i < 8; i++)
		swap_map[dummy_byte[i]] = i;

	for (i = 0; i < 8; i++) {
		if (i == swap_map[i]) {
			to_check[i] = false;
		} else if (to_check[i]) {
			current = i;
			for (next = swap_map[i]; next != current; next = swap_map[next]) {
				to_check[next] = false;
				okey[okey_size] = (0x01 << current) | (0x01 << next);
				okey_size++;
			}
		}
	}

	*okey_size_ptr = okey_size;
}


/**********************
 **      PUBLIC      **
 **********************/

bool mbc_set_user_key(const uint8_t* key, size_t key_size) {
	xor_key = malloc(key_size);
	if (xor_key == NULL)
		return false;

	memcpy(xor_key, key, key_size);
	xor_key_size = key_size;

	make_swap_key(swap_key, &swap_key_size, key, key_size);

	return true;
}

void mbc_free(void) {
	free(xor_key);
	xor_key = NULL;
	xor_key_size = 0;
	swap_key_size = 0;
}

void mbc_encode_inplace(uint8_t* data, size_t data_size) {
	register size_t i, j;

	/* SWAP */
	for (i = 0; i < data_size; i++)
		for (j = 0; j < swap_key_size; j++)
			if (((data[i] & swap_key[j]) != swap_key[j]) && ((data[i] & swap_key[j]) != 0x00))
				data[i] ^= swap_key[j];

	/* XOR */
	for (i = 0; i < data_size; i++)
		data[i] ^= xor_key[i % xor_key_size];
	if (data_size > 0)
		for (; i < xor_key_size; i++)
			data[i % data_size] ^= xor_key[i];
}

void mbc_decode_inplace(uint8_t* data, size_t data_size) {
	register size_t i;
	register int_fast8_t j;

	/* XOR */
	for (i = 0; i < data_size; i++)
		data[i] ^= xor_key[i % xor_key_size];
	if (data_size > 0)
		for (; i < xor_key_size; i++)
			data[i % data_size] ^= xor_key[i];

	/* SWAP */
	for (i = 0; i < data_size; i++)
		for (j = swap_key_size-1; j >= 0; j--)
			if (((data[i] & swap_key[j]) != swap_key[j]) && ((data[i] & swap_key[j]) != 0x00))
				data[i] ^= swap_key[j];
}

uint8_t* mbc_encode(const uint8_t* data, size_t data_size) {
	uint8_t* edata;

	edata = malloc(data_size);
	if (edata == NULL)
		return NULL;

	memcpy(edata, data, data_size);
	mbc_encode_inplace(edata, data_size);

	return edata;
}

uint8_t* mbc_decode(const uint8_t* data, size_t data_size) {
	uint8_t* ddata;

	ddata = malloc(data_size);
	if (ddata == NULL)
		return NULL;

	memcpy(ddata, data, data_size);
	mbc_decode_inplace(ddata, data_size);

	return ddata;
}

char* mbc_encode_to_hex(const uint8_t* raw_in, size_t raw_size, bool uppercase) {
	uint8_t* encoded_data;
	char* hex_out;

	encoded_data = mbc_encode(raw_in, raw_size);
	if (encoded_data == NULL)
		return NULL;

	hex_out = mbc_raw_to_hex(encoded_data, raw_size, uppercase);
	free(encoded_data);

	return hex_out;
}

uint8_t* mbc_decode_from_hex(const char* hex_in, size_t* decoded_size_ptr) {
	uint8_t *raw_data;
	size_t raw_size;

	raw_data = mbc_hex_to_raw(hex_in, &raw_size);
	if (raw_data == NULL)
		return NULL;

	mbc_decode_inplace(raw_data, raw_size);
	*decoded_size_ptr = raw_size;

	return raw_data;
}

char* mbc_raw_to_hex(const uint8_t* raw, size_t raw_size, bool uppercase) {
	char* hex;
	size_t hex_size;
	register size_t i;

	hex_size = raw_size * 2 + 1;
	hex      = malloc(hex_size);
	if (hex == NULL)
		return NULL;

	const char (*HEXMAP)[2] = uppercase ? HEXMAP_UPPER : HEXMAP_LOWER;

	for (i = 0; i < raw_size; i++)
		memcpy(hex + i*2, HEXMAP[raw[i]], 2);

	hex[hex_size-1] = '\0';

	return hex;
}

uint8_t* mbc_hex_to_raw(const char* hex, size_t* raw_size_ptr) {
	uint8_t* raw;
	size_t hex_size, raw_size;
	register size_t i;

	hex_size = strlen(hex);
	raw_size = hex_size / 2;
	raw      = malloc(raw_size);
	if (raw == NULL)
		return NULL;

	for (i = 0; i < raw_size; i++)
		raw[i] = HEXMAP_REVERSE[(uint8_t)hex[i*2]] * 0x10 + HEXMAP_REVERSE[(uint8_t)hex[i*2 + 1]];

	*raw_size_ptr = raw_size;

	return raw;
}
