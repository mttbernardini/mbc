#include <stdbool.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include "mbc.h"

/* ^ TODO: check includes. */

/***********************
 **      PRIVATE      **
 ***********************/

static uint8_t* user_key;
static size_t user_key_len;
static uint8_t* oct_key;
static size_t oct_key_len;
static enum mbc_options user_options;

/**
 * Generates the octal key (to be used in misc phase) from the user key.
 * @pre  Global `user_key` contains a valid user key.
 * @post Global `oct_key` is now the octal key.
 */
static bool make_oct_key();

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

static bool make_oct_key() {
	register size_t i;
	size_t key_size;
	uint8_t* okey;

	key_size     = strlen(key);
	okey         = malloc(key_size);

	for (i = 0; i < key_size; i++) {
		okey[i] = (key[i] & 0x70) + ((key[i] >> 1) & 0x07);
		if (key[i] & 0x1) okey[i] = ~okey[i];
	}

	*okey_size_p = key_size;

	return okey;
}

static char* raw_to_hex(const uint8_t* raw, size_t raw_size, bool uppercase) {
	char* hex;
	size_t hex_size;
	register size_t i;

	hex_size = raw_size * 2 + 1;
	hex = malloc(hex_size);

	if (uppercase)
		for (i = 0; i < hex_size; i++) sprintf(hex, "%2F", raw[i]);
	else
		for (i = 0; i < hex_size; i++) snprintf(hex, "%2f", raw[i]);
	
	hex[hex_size] = '\0';

	return hex;
}

/* ANYTHING AFTER THIS STILL NEEDS TO BE CHECKED */

static uint8_t* hex_to_raw(const char* hex, size_t* raw_size_p) {
	uint8_t* raw;
	size_t hex_size;
	register size_t i, shift;

	hex_size	= strlen(hex);
	*raw_size_p = hex_size/2;
	raw	        = calloc(1, *raw_size_p);

	for (i = 0, shift = 1; i < hex_size; i++, shift^=1) {
		if (hex[i] >= '0' && hex[i] <= '9')
			raw[i/2] += (hex[i] - '0')       << (shift ? 4 : 0);
		else if (hex[i] >= 'a' && hex[i] <= 'f')
			raw[i/2] += (hex[i] - 'a' + 0xA) << (shift ? 4 : 0);
		else if (hex[i] >= 'A' && hex[i] <= 'F')
			raw[i/2] += (hex[i] - 'A' + 0xA) << (shift ? 4 : 0);
	}

	return raw;
}


/**********************
 **      PUBLIC      **
 **********************/

enum mbc_options {
	MBC_ENCODE_HEX_IN  = 0x1,
	MBC_ENCODE_HEX_OUT = 0x2,
	MBC_DECODE_HEX_IN  = 0x4,
	MBC_DECODE_HEX_OUT = 0x8
};

bool mbc_set_user_key(const uint8_t* key, size_t key_size) {
	
	/* TODO */

	return true;
}

bool mbc_set_options(enum mbc_options options) {
	
	/* TODO */
	
	return true;
}

void* mbc_encode(uint8_t* data, const char* xkey, const uint8_t* okey, size_t data_size, size_t okey_size) {
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
	}
}


void* mbc_decode(uint8_t* data, const char* xkey, const uint8_t* okey, size_t data_size, size_t okey_size) {
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
	}
}

void mbc_free() {
	
	/* TODO */

}
