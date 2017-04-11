#include "libmbc.h"
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
	register size_t i;
	size_t okey_size;
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

void mbc_encode_inplace(uint8_t* data, size_t data_size) {
	register size_t i, j;

	// XOR
	for (i = 0; i < data_size; i++)
		data[i] ^= user_key[i % user_key_size];
	for (; i < user_key_size; i++)
		data[i % data_size] ^= user_key[i];

	// SWAP
	for (i = 0; i < data_size; i++) {
		for (j = 0; j < oct_key_size; j++)
			if (data[i] >> oct_key[j][0] != data[i] >> oct_key[j][1])
				data[i] ^= (0x01 << oct_key[j][0]) ^ (0x01 << oct_key[j][1]);
	}
}

void mbc_decode_inplace(uint8_t* data, size_t data_size) {
	register size_t i;
	register int8_t j;

	// SWAP
	for (i = 0; i < data_size; i++) {
		for (j = oct_key_size-1; j >= 0; j--)
			if (data[i] >> oct_key[j][0] != data[i] >> oct_key[j][1])
				data[i] ^= (0x01 << oct_key[j][0]) ^ (0x01 << oct_key[j][1]);
	}

	// XOR
	for (i = 0; i < data_size; i++)
		data[i] ^= user_key[i % user_key_size];
	for (; i < user_key_size; i++)
		data[i % data_size] ^= user_key[i];
}


// EXTRA

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


// UTILITIES

char* mbc_raw_to_hex(const uint8_t* raw, size_t raw_size, bool uppercase) {
	char* hex;
	size_t hex_size;
	register size_t i;

	hex_size = raw_size * 2 + 1;
	hex = malloc(hex_size);
	if (hex == NULL)
		return NULL;

	if (uppercase)
		for (i = 0; i < raw_size; i++)
			sprintf(hex + i*2, "%02X", raw[i]);
	else
		for (i = 0; i < raw_size; i++)
			sprintf(hex + i*2, "%02x", raw[i]);

	hex[hex_size-1] = '\0';

	return hex;
}

uint8_t* mbc_hex_to_raw(const char* hex, size_t* raw_size_ptr) {
	uint8_t* raw;
	unsigned int temp;
	size_t hex_size, raw_size;
	register size_t i;

	hex_size      = strlen(hex);
	raw_size      = hex_size/2;
	raw           = malloc(raw_size);
	if (raw == NULL)
		return NULL;

	for (i = 0; i < raw_size; i++) {
		sscanf(hex + i*2, "%2x", &temp);
		raw[i] = (uint8_t) temp;
	}

	*raw_size_ptr = raw_size;

	return raw;
}
