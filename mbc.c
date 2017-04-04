#include <stdbool.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h>

static char* user_key;
static uint8_t* oct_key;

uint8_t* make_oct_key(const char* key, size_t* okey_size_p) {
	/**
	 * Generates the octal key to be used in misc phase from the encryption key.
	 * @ret  `okey` is the new octal key
	 * @pre  `key` length > 0 (otherwhise malloc() is undef behavior)
	 */

	/*********** CONVERT: USE GLOBAL KEYS ***********/

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

	/****

	-- RATIONALE --

	EXTRACT KEY BITS:
	key[i]            : -xxxyyy- &
    bit mask L (0x70) : 01110000 =
	okey[i]           = 0xxx0000

	key[i]            : -xxxyyy- >> 1 =
	                    0-xxxyyy &
	bit mask R (0x07) : 00001110 =
	okey[i]          += 00000yyy =
	okey[i]           : 0xxx0yyy

	IF (key[i] & 0x01):
	okey[i] = ~okey[i]
	(specular positions)

	****/

	return okey;
}


char* fit_hex_key(const char* key, size_t max_key_size) {
	/**
	 * XORES the key against itself if it's longer than `max_key_size`
	 * @ret  `fkey` is the new key
	 * @pre  `key` length > 0, `max_key_size` > 0
	 * @post `fkey` length <= `max_key_size`
	 */

	/*********** CONVERT: USE GLOBAL KEYS ***********/

	size_t key_size;
	register size_t i;
	char* fkey;

	key_size = strlen(key);

	if (key_size > max_key_size) {
		fkey = malloc(max_key_size + 1);
		fkey[max_key_size] = '\0';
		memcpy(fkey, key, max_key_size);

		for (i = max_key_size; i < key_size; i++)
			fkey[i % max_key_size] ^= key[i];

	} else {
		fkey = malloc(key_size + 1);
		fkey[key_size] = '\0';
		memcpy(fkey, key, key_size);
	}

	return fkey;

}


char* raw_to_hex(const uint8_t* raw, size_t raw_size) {
	/**
	 * Converts raw data into an hexadecimal string.
	 * @ret  `hex` string
	 * @pre  `raw` contains raw bytes, `raw_size` > 0 (malloc() undef behavior otherwhise)
	 * @post `hex` length is even, containing only uppercase hex chars (see map)
	 */

	/*********** CONVERT: DON'T USE MAP, CHECK `snprintf` ***********/

	uint8_t* hex;
	register size_t i, j;

	const uint8_t hex_map[16] = {
		'0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'A', 'B', 'C', 'D', 'E', 'F'
	};

	hex = malloc(raw_size*2 + 1);
	hex[raw_size*2] = '\0';

	for (i = 0, j = 0; i < raw_size; i++, j += 2) {
		hex[j]	 = hex_map[(raw[i] >> 4) & 0x0F];
		hex[j+1] = hex_map[ raw[i]	     & 0x0F];
	}

	return hex;
}


uint8_t* hex_to_raw(const char* hex, size_t* raw_size_p) {
	/**
	 * Converts hexadecimal string into raw data.
	 * @ret  `raw` data array
	 * @pre  `hex` length should be even and > 0, containing valid lower/uppercase hex digits
	 * @post `*raw_size_p` contains the size of `raw`
	 */

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


void encoder(uint8_t* data, const char* xkey, const uint8_t* okey, size_t data_size, size_t okey_size) {
	/**
	 * Encodes an array of raw bytes, using 2 keys.
	 * TODO: @ret Should we return the data instead of modifying it directly?
	 * @pre  `data_size` > 0, `xkey` length <= `data_size` (use fit_hex_key before, otherwhise overflow bytes are ignored)
	 * @post `data` is encoded
	 */

	/*********** CONVERT: USE GLOBAL KEYS ***********/

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


void decoder(uint8_t* data, const char* xkey, const uint8_t* okey, size_t data_size, size_t okey_size) {
	/**
	 * Decodes an array of raw bytes, using 2 keys.
	 * @pre  `data_size` > 0, `xkey` length <= `data_size` (use fit_hex_key before, otherwhise overflow bytes are ignored)
	 * @post `data` is decoded
	 */

	/*********** CONVERT: USE GLOBAL KEYS *************/

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
