#include <stdbool.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include "mbc.h"

/***********************
 **      PRIVATE      **
 ***********************/

static uint8_t* user_key;
static size_t user_key_len;
static uint8_t* oct_key;
static size_t oct_key_len;

/**
 * Generates the octal key (to be used in misc phase) from the user key.
 * @pre  Global `user_key` contains a valid user key.
 * @post Global `oct_key` is now the octal key.
 */
static bool make_oct_key();


static bool make_oct_key() {
	register size_t i;
	size_t key_size;
	uint8_t *okey, *key;

	key      = user_key;
	key_size = user_key_len;
	okey     = malloc(key_size);

	for (i = 0; i < key_size; i++) {
		okey[i] = (key[i] & 0x70) + ((key[i] >> 1) & 0x07);
		if (key[i] & 0x1) okey[i] = ~okey[i];
	}

	// NB: what if unbind this function from the global vars
	//     and have set_user_key do that?
	oct_key     = okey;
	oct_key_len = key_size;

	return true;
}


/**********************
 **      PUBLIC      **
 **********************/

bool mbc_set_user_key(const uint8_t* key, size_t key_size) {
	user_key = malloc(key_size);
	if (user_key == NULL)
		return false;

	memcpy(user_key, key, key_size);
	user_key_len = key_size;

	return make_oct_key();
}

void mbc_free() {
	free(user_key);
	free(oct_key);
	user_key_len = 0;
	oct_key_len  = 0;
}

uint8_t* mbc_encode(const uint8_t* data, size_t data_size) {
	uint8_t *xkey, *okey, *edata;
	size_t xkey_size, okey_size;
	register size_t i, j;
	uint8_t l_bit_pos, r_bit_pos;

	xkey      = user_key;
	okey      = oct_key;
	xkey_size = user_key_len;
	okey_size = oct_key_len;

	edata = malloc(data_size); // FIXME: handle failure
	memcpy(edata, data, data_size);

	// XOR PART
	for (i = 0; i < data_size; i++) {
		edata[i] ^= xkey[i % xkey_size];
	}
	if (xkey_size > data_size)
		for (; i < xkey_size; i++) {
			edata[i % data_size] ^= xkey[i];
		}

	// MISC PART
	for (i = 0; i < data_size; i++) {
		for (j = 0; j < okey_size; j++) {
			l_bit_pos = (okey[j] & 0x70) >> 4;
			r_bit_pos = (okey[j] & 0x07);

			if (((edata[i] >> l_bit_pos) & 0x1) != ((edata[i] >> r_bit_pos) & 0x1))
				edata[i] ^= (0x1 << l_bit_pos) ^ (0x1 << r_bit_pos);
		}
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
	xkey_size = user_key_len;
	okey_size = oct_key_len;

	ddata = malloc(data_size); // FIXME: handle failure
	memcpy(ddata, data, data_size);

	// MISC PART
	for (i = 0; i < data_size; i++) {
		for (j = okey_size-1; j < okey_size; j--) {
			l_bit_pos = (okey[j] & 0x70) >> 4;
			r_bit_pos = (okey[j] & 0x07);

			if (((ddata[i] >> l_bit_pos) & 0x1) != ((ddata[i] >> r_bit_pos) & 0x1))
				ddata[i] ^= (0x1 << l_bit_pos) ^ (0x1 << r_bit_pos);
		}
	}

	// XOR PART
	for (i = 0; i < data_size; i++) {
		ddata[i] ^= xkey[i % xkey_size];
	}
	if (xkey_size > data_size)
		for (; i < xkey_size; i++) {
			ddata[i % data_size] ^= xkey[i];
		}

	return ddata;
}

char* mbc_raw_to_hex(const uint8_t* raw, size_t raw_size, bool uppercase) {
	char* hex;
	size_t hex_size;
	register size_t i;

	hex_size = raw_size * 2 + 1;
	hex = malloc(hex_size);  //FIXME: handle malloc error

	if (uppercase)
		for (i = 0; i < hex_size; i++) sprintf(hex, "%2F", raw[i]);
	else
		for (i = 0; i < hex_size; i++) snprintf(hex, "%2f", raw[i]);

	hex[hex_size] = '\0';

	return hex;
}

// FIXME: don't require even-length input, as now this is a public function and should be more tolerant.
uint8_t* mbc_hex_to_raw(const char* hex, size_t* raw_size_p) {
	uint8_t* raw;
	size_t hex_size;
	register size_t i, shift;

	hex_size	= strlen(hex);
	*raw_size_p = hex_size/2;
	raw	        = calloc(1, *raw_size_p);  //FIXME: handle calloc error

	// TODO: use sscanf()
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
