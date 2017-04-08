#include "mbc.h"
#include <inttypes.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

/***********************
 **      PRIVATE      **
 ***********************/

typedef uint8_t mbc_oct_key_el_t[2];
typedef mbc_oct_key_el_t* mbc_oct_key_t;

static uint8_t* user_key;
static size_t user_key_size;
static mbc_oct_key_t oct_key;
static size_t oct_key_size;

/**
 * Generates the octal key (to be used in misc phase) from a user key passed as parameter.
 * @ret  Generated octal key, `NULL` if the key cannot be `malloc`ated.
 * @post `*okey_size_ptr` now contains the size of the `oct_key` generated.
 */
static mbc_oct_key_t make_oct_key(const uint8_t* key, size_t key_size, size_t* okey_size_ptr) {
	uint8_t l_bit, r_bit, swap_temp, current, next;
	uint8_t swap_mask[8] = {0,1,2,3,4,5,6,7};
	bool to_check[8] = {1,1,1,1,1,1,1,1};
	register size_t i, j;
	size_t okey_size;
	mbc_oct_key_el_t to_find;
	mbc_oct_key_t okey;

	for (i = 0; i < key_size; i++) {
		l_bit = (key[i] >> 4) & 0x07;
		r_bit = (key[i] >> 1) & 0x07;

		if (l_bit != r_bit) {
			if ((key[i] & 0x01) && l_bit != (r_bit ^ 0x07)) {
				l_bit ^= 0x07;
				r_bit ^= 0x07;
			}

			swap_temp = swap_mask[l_bit];
			swap_mask[l_bit] = swap_mask[r_bit];
			swap_mask[r_bit] = swap_temp;
		}
	}

	okey = malloc(sizeof(mbc_oct_key_el_t) * 8);
	okey_size = 0;
	for (i = 0; i < 8; i++) {
		if (i == swap_mask[i]) {
			to_check[i] = false;
		} else if (to_check[i]) {
			current = i;
			for (next = swap_mask[i]; next != current; next = swap_mask[next]) {
				to_check[next] = false;
				okey[okey_size][0] = current;
				okey[okey_size][1] = next;
				okey_size++;
			}
		}
	}

	*okey_size_ptr = okey_size;
	return okey;
}


/**********************
 **      PUBLIC      **
 **********************/

bool mbc_set_user_key(const uint8_t* key, size_t key_size) {
	user_key = malloc(key_size);
	if (user_key == NULL)
		return false;

	memcpy(user_key, key, key_size);
	user_key_size = key_size;
	oct_key = make_oct_key(user_key, user_key_size, &oct_key_size);
	if (oct_key == NULL)
		return false;

	return true;
}

void mbc_free() {
	free(user_key);
	user_key = NULL;
	user_key_size = 0;
	free(oct_key);
	oct_key = NULL;
	oct_key_size = 0;
}

uint8_t* mbc_encode(const uint8_t* data, size_t data_size) {
	uint8_t* edata;
	register size_t i, j;

	edata = malloc(data_size);
	if (edata == NULL)
		return NULL;

	// XOR
	for (i = 0; i < data_size; i++)
		edata[i] = data[i] ^ user_key[i % user_key_size];
	if (user_key_size > data_size)
		for (; i < user_key_size; i++)
			edata[i % data_size] ^= user_key[i];

	// SWAP
	for (i = 0; i < data_size; i++)
		for (j = 0; j < oct_key_size; j++)
			if (edata[i] >> oct_key[j][0] != edata[i] >> oct_key[j][1])
				edata[i] ^= (0x01 << oct_key[j][0]) ^ (0x01 << oct_key[j][1]);

	return edata;
}

uint8_t* mbc_decode(const uint8_t* data, size_t data_size) {
	uint8_t* ddata;
	register size_t i;
	register int8_t j;

	ddata = malloc(data_size);
	if (ddata == NULL)
		return NULL;

	// SWAP
	for (i = 0; i < data_size; i++)
		for (j = oct_key_size-1; j >= 0; j++)
			if (data[i] >> oct_key[j][0] != data[i] >> oct_key[j][1])
				ddata[i] = data[i] ^ (0x01 << oct_key[j][0]) ^ (0x01 << oct_key[j][1]);

	// XOR
	for (i = 0; i < data_size; i++)
		ddata[i] ^= user_key[i % user_key_size];
	if (user_key_size > data_size)
		for (; i < user_key_size; i++)
			ddata[i % data_size] ^= user_key[i];

	return ddata;
}

char* mbc_raw_to_hex(const uint8_t* raw, size_t raw_size, bool uppercase) {
	char* hex;
	size_t hex_size;
	register size_t i;

	hex_size = raw_size * 2 + 1;
	hex = malloc(hex_size);
	if (hex == NULL)
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

	// FIXME: This is wrong, can't use "%2x"SCNu8 to scan for hex uint8_t...
	for (i = 0; i < raw_size; i++) {
		sscanf(hex + i*2, "%2x" SCNu8, raw + i);
	}

	return raw;
}
