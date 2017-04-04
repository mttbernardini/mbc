#include <stdbool.h>
#include <inttypes.h>
#include <string.h>
#include <stdlib.h>
#include "mbc.h"

/***********************
 **      PRIVATE      **
 ***********************/

static uint8_t* user_key;
static size_t user_key_size;
static uint8_t* oct_key;
static const size_t oct_key_size = 8;
static enum mbc_options_enum user_options = MBC_OPT_NONE;

/**
 * Generates the octal key (to be used in misc phase) from a user key passed as parameter.
 * @ret Generated octal key.
 */
static uint8_t* make_oct_key(const uint8_t* key, size_t key_size);

/**
 * Converts raw data into an hexadecimal string.
 * @ret  NULL-terminated hexadecimal string.
 * @pre  `raw` contains raw bytes, `raw_size` is the size of `raw` and should be > 0.
 * @post The length of the returned string is even, containing only lower/uppercase (accordingly to `uppercase`) hexadecimal ASCII characters.
 */
static char* raw_to_hex(const uint8_t* raw, size_t raw_size, bool uppercase);

/**
 * Converts hexadecimal string into raw data.
 * @ret  Raw data array.
 * @pre  `hex` is a NULL-terminated string containing only lower/uppercase hexadecimal ASCII characters, and its length should be even and > 0.
 * @post `*raw_size_ptr` contains the size of the raw data returned.
 */
static uint8_t* hex_to_raw(const char* hex, size_t* raw_size_ptr);


static uint8_t* make_oct_key(const uint8_t* key, size_t key_size) {
	uint8_t l_bit, r_bit, okey[] = {0, 1, 2, 3, 4, 5, 6, 7};
	size_t swap_temp;
	register size_t i;

	for (i = 0; i < key_size; i++) {
		l_bit = (key[i] >> 4) & 0x07;
		r_bit = (key[i] >> 1) & 0x07;

		if (l_bit != r_bit) {
			if ((key[i] & 0x01) && l_bit != (r_bit ^ 0x07)) {
				l_bit ^= 0x07;
				r_bit ^= 0x07;
			}

			swap_temp = okey[l_bit];
			okey[l_bit] = okey[r_bit];
			okey[r_bit] = swap_temp;
		}
	}

	return okey;
}

static char* raw_to_hex(const uint8_t* raw, size_t raw_size, bool uppercase) {
	char* hex;
	size_t hex_size;
	register size_t i;

	hex_size = raw_size * 2 + 1;
	hex = malloc(hex_size);

	if (uppercase)
		for (i = 0; i < hex_size; i++)
			sprintf(hex, "%2X", raw[i]);
	else
		for (i = 0; i < hex_size; i++)
			sprintf(hex, "%2x", raw[i]);

	hex[hex_size] = '\0';

	return hex;
}

static uint8_t* hex_to_raw(const char* hex, size_t* raw_size_ptr) {
	uint8_t* raw;
	size_t hex_size;
	register size_t i;

	hex_size = strlen(hex);
	*raw_size_ptr = hex_size/2;
	raw = malloc(*raw_size_ptr);

	for (i = 0; i < raw_size; i++) {
		sscanf(hex + i*2, "%2x" SCNu8, raw + i);
	}

	return hex;
}


/**********************
 **      PUBLIC      **
 **********************/

bool mbc_set_user_key(const uint8_t* key, size_t key_size) {

	user_key_size = key_size;
	user_key = malloc(user_key_size);

	if (user_key == NULL) return false;

	user_key = memcpy(user_key, key, user_key_size);
	oct_key = make_oct_key(user_key, user_key_size);

	return true;
}

bool mbc_set_options(enum mbc_options_enum options) {

	/* TODO */

	return true;
}

void* mbc_encode(void* data, size_t data_size, size_t* out_data_size) {
	// old params: uint8_t* data, const char* xkey, const uint8_t* okey, size_t data_size, size_t okey_size

	/* FIXME, steps:
	 * 1. eventually decode hex input
	 * 2. loop over user_key or data accordingly and do xor part
	 * 3. do misc part
	 * 4. eventually encode hex output
	 *

	size_t xkey_size;
	register size_t i, j;
	uint8_t l_bit_pos, r_bit_pos;

	xkey_size = strlen(xkey);

	// XOR PART
	for (i = 0; i < data_size; i++) {
		data[i] ^= xkey[i % xkey_size];
	}

	// MISC PART
	for (i = 0; i < data_size; i++) {
		for (j = 0; j < okey_size; j++) {
			l_bit_pos = (okey[j] & 0x70) >> 4;
			r_bit_pos = (okey[j] & 0x07);

			if (((data[i] >> l_bit_pos) & 0x1) != ((data[i] >> r_bit_pos) & 0x1))
				data[i] ^= (0x1 << l_bit_pos) ^ (0x1 << r_bit_pos);
		}
	}*/

	return NULL;
}

void* mbc_decode(void* data, size_t data_size, size_t* out_data_size) {
	// old params: uint8_t* data, const char* xkey, const uint8_t* okey, size_t data_size, size_t okey_size

	/* FIXME, steps:
	 * 1. eventually decode hex input
	 * 2. do misc part in reverse
	 * 3. loop over user_key or data accordingly and do xor part
	 * 4. eventually encode hex output
	 *

	size_t xkey_size;
	register size_t i, j;
	uint8_t l_bit_pos, r_bit_pos;

	xkey_size = strlen(xkey);

	// MISC PART
	for (i = 0; i < data_size; i++) {
		for (j = okey_size-1; j < okey_size; j--) {
			l_bit_pos = (okey[j] & 0x70) >> 4;
			r_bit_pos = (okey[j] & 0x07);

			if (((data[i] >> l_bit_pos) & 0x1) != ((data[i] >> r_bit_pos) & 0x1))
				data[i] ^= (0x1 << l_bit_pos) ^ (0x1 << r_bit_pos);
		}
	}

	// XOR PART
	for (i = 0; i < data_size; i++) {
		data[i] ^= xkey[i % xkey_size];
	}*/

	return NULL;
}

void mbc_free() {

	/* TODO */

}
