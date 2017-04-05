#include <stdbool.h>
#include <inttypes.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "mbc.h"

/***********************
 **      PRIVATE      **
 ***********************/

static uint8_t* user_key;
static size_t user_key_size;
static uint8_t* oct_key;
static const size_t oct_key_size = 8;

/**
 * Generates the octal key (to be used in misc phase) from a user key passed as parameter.
 * @ret Generated octal key.
 */
static uint8_t* make_oct_key(const uint8_t* key, size_t key_size) {
	uint8_t l_bit, r_bit, *okey;
	size_t swap_temp;
	register size_t i;

	okey = malloc(oct_key_size);
	for (i = 0; i < oct_key_size; i++) {
		okey[i] = (uint8_t) i;
	}

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


/**********************
 **      PUBLIC      **
 **********************/

bool mbc_set_user_key(const uint8_t* key, size_t key_size) {
	user_key = malloc(key_size);

	if (user_key == NULL)
		return false;

	user_key      = memcpy(user_key, key, key_size);
	user_key_size = key_size;
	oct_key       = make_oct_key(user_key, user_key_size);

	return true;
}

void mbc_free() {
	free(user_key);
	free(oct_key);
	user_key = NULL;
	oct_key = NULL;
	user_key_size = 0;
}

uint8_t* mbc_encode(const uint8_t* data, size_t data_size) {
	uint8_t *xkey, *okey, *edata;
	size_t xkey_size, okey_size;
	register size_t i, j;
	uint8_t temp_byte;

	xkey      = user_key;
	okey      = oct_key;
	xkey_size = user_key_size;
	okey_size = oct_key_size;

	edata = malloc(data_size);
	if (edata == NULL)
		return NULL;

	// XOR PART
	for (i = 0; i < data_size; i++)
		edata[i] = data[i] ^ xkey[i % xkey_size];
	if (xkey_size > data_size)
		for (; i < xkey_size; i++)
			edata[i % data_size] ^= xkey[i];

	// MISC PART
	for (i = 0; i < data_size; i++) {
		temp_byte = 0x00;
		for (j = 0; j < okey_size; j++)
			if (j != okey[j])
				temp_byte |= ((edata[i] >> j) & 0x01) << okey[j];
		edata[i] = temp_byte;
	}

	return edata;
}

uint8_t* mbc_decode(const uint8_t* data, size_t data_size) {
	uint8_t *xkey, *okey, *ddata;
	size_t xkey_size, okey_size;
	register size_t i, j;
	uint8_t l_bit_pos, r_bit_pos;

	xkey      = user_key;
	okey      = oct_key;
	xkey_size = user_key_size;
	okey_size = oct_key_size;

	ddata = malloc(data_size);
	if (ddata == NULL)
		return NULL;

	// MISC PART
	for (i = 0; i < data_size; i++) {
		ddata[i] = 0x00;
		for (j = 0; j < okey_size; j++)
			if (j != okey[j])
				ddata[i] |= ((ddata[i] >> okey[j]) & 0x01) << j;
	}

	// XOR PART
	for (i = 0; i < data_size; i++)
		ddata[i] ^= xkey[i % xkey_size];
	if (xkey_size > data_size)
		for (; i < xkey_size; i++)
			ddata[i % data_size] ^= xkey[i];

	return ddata;
}

char* mbc_raw_to_hex(const uint8_t* raw, size_t raw_size, bool uppercase) {
	char* hex;
	size_t hex_size;
	register size_t i;

	hex_size = raw_size * 2 + 1;
	hex = malloc(hex_size);
	if (hex_size == NULL)
		return NULL;

	if (uppercase)
		for (i = 0; i < hex_size; i++)
			sprintf(hex, "%2X", raw[i]);
	else
		for (i = 0; i < hex_size; i++)
			sprintf(hex, "%2x", raw[i]);

	hex[hex_size] = '\0';

	return hex;
}

uint8_t* mbc_hex_to_raw(const char* hex, size_t* raw_size_ptr) {
	uint8_t* raw;
	size_t hex_size, raw_size;
	register size_t i;

	hex_size      = strlen(hex);
	*raw_size_ptr = hex_size/2;
	raw_size      = *raw_size_ptr;
	raw           = malloc(raw_size);
	if (raw == NULL)
		return NULL;

	for (i = 0; i < raw_size; i++) {
		sscanf(hex + i*2, "%2x" SCNu8, raw + i);
	}

	return raw;
}
