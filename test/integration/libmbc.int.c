#include <time.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include <ctest.h>
#include <libmbc.h>

static const size_t MIN_KEY_SIZE      = 32L;
static const size_t MAX_KEY_SIZE      = 1024L * 1024L * 32L;
static const size_t MIN_RAW_DATA_SIZE = 128L;
static const size_t MAX_RAW_DATA_SIZE = 1024L * 1024L * 128L;
static const char   HEX_CHARS_UPPER[] = {'0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'A', 'B', 'C', 'D', 'E', 'F'};
static const char   HEX_CHARS_LOWER[] = {'0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'a', 'b', 'c', 'd', 'e', 'f'};

static uint8_t* generate_random_key(size_t* key_size) {
	size_t i;
	uint8_t* key;

	if (*key_size == 0)
		*key_size = (size_t)(rand() % (MAX_KEY_SIZE - MIN_KEY_SIZE) + MIN_KEY_SIZE + 1);

	CTEST_LOG("key: %zuB", *key_size);

	key = malloc(*key_size);
	for (i = 0; i < *key_size; i++)
		key[i] = (uint8_t)(rand() % 0xFF);

	return key;
}

static void* generate_random_data(void* expected, size_t* data_size, bool hex, bool uppercase) {
	register size_t i;

	if (*data_size == 0)
		*data_size = (size_t)(rand() % (MAX_RAW_DATA_SIZE - MIN_RAW_DATA_SIZE) + MIN_RAW_DATA_SIZE + 1);

	CTEST_LOG("data (%s): %zuB.", hex ? (uppercase ? "HEX" : "hex") : "raw", *data_size);

	if (hex) {
		const char* HEX_CHARS = uppercase ? HEX_CHARS_UPPER : HEX_CHARS_LOWER;
		char *hex_data, *hex_exp;

		hex_data = malloc(*data_size + 1);
		hex_exp  = malloc(*data_size + 1);

		for (i = 0; i < *data_size; i++)
			hex_data[i] = HEX_CHARS[rand() % 0x10];

		hex_data[*data_size] = '\0';
		memcpy(hex_exp, hex_data, *data_size + 1);

		if (*data_size % 2 != 0)
			hex_exp[*data_size - 1] = '\0';

		*(char**)expected = hex_exp;
		return hex_data;

	} else {
		uint8_t *raw_data, *raw_exp;
		raw_data = malloc(*data_size);
		raw_exp  = malloc(*data_size);

		for (i = 0; i < *data_size; i++)
			raw_data[i] = (uint8_t)(rand() % 0xFF);

		memcpy(raw_exp, raw_data, *data_size);
		*(uint8_t**) expected = raw_exp;
		return raw_data;
	}
}

CTEST(libmbc, encode_decode__inplace) {
	uint8_t *k, *d, *e;
	size_t ks, ds;

	ks = 0; ds = 0;
	k = generate_random_key(&ks);
	d = generate_random_data(&e, &ds, false, false);

	mbc_set_user_key(k, ks);
	mbc_encode_inplace(d, ds);
	mbc_decode_inplace(d, ds);
	ASSERT_DATA(e, ds, d, ds);

	free(k);
	free(d);
	free(e);
	mbc_free();
}

CTEST(libmbc, encode_decode) {
	uint8_t *k, *d, *de, *dd, *e;
	size_t ks, ds;

	ks = 0; ds = 0;
	k = generate_random_key(&ks);
	d = generate_random_data(&e, &ds, false, false);

	mbc_set_user_key(k, ks);
	de = mbc_encode(d, ds);
	dd = mbc_decode(de, ds);
	ASSERT_DATA(e, ds, dd, ds);

	free(k);
	free(d);
	free(de);
	free(dd);
	free(e);
	mbc_free();
}

CTEST(libmbc, encode_decode__to_from_hex__lower) {
	uint8_t *k, *d, *dd, *e;
	char *dex;
	size_t ks, ds, dds;

	ks = 0; ds = 0;
	k = generate_random_key(&ks);
	d = generate_random_data(&e, &ds, false, false);

	mbc_set_user_key(k, ks);
	dex = mbc_encode_to_hex(d, ds, false);
	dd = mbc_decode_from_hex(dex, &dds);
	ASSERT_DATA(e, ds, dd, dds);

	free(k);
	free(d);
	free(dex);
	free(dd);
	free(e);
	mbc_free();
}

CTEST(libmbc, encode_decode__to_from_hex__upper) {
	uint8_t *k, *d, *dd, *e;
	char *dex;
	size_t ks, ds, dds;

	ks = 0; ds = 0;
	k = generate_random_key(&ks);
	d = generate_random_data(&e, &ds, false, false);

	mbc_set_user_key(k, ks);
	dex = mbc_encode_to_hex(d, ds, true);
	dd = mbc_decode_from_hex(dex, &dds);
	ASSERT_DATA(e, ds, dd, dds);

	free(k);
	free(d);
	free(dex);
	free(dd);
	free(e);
	mbc_free();
}

CTEST(libmbc, convert__raw_to_hex_to_raw__lower) {
	uint8_t *d, *dc, *e;
	char *dx;
	size_t ds, dcs;

	ds = 0;
	d = generate_random_data(&e, &ds, false, false);

	dx = mbc_raw_to_hex(d, ds, false);
	dc = mbc_hex_to_raw(dx, &dcs);
	ASSERT_DATA(e, ds, dc, dcs);

	free(d);
	free(dc);
	free(dx);
	free(e);
}

CTEST(libmbc, convert__raw_to_hex_to_raw__upper) {
	uint8_t *d, *dc, *e;
	char *dx;
	size_t ds, dcs;

	ds = 0;
	d = generate_random_data(&e, &ds, false, false);

	dx = mbc_raw_to_hex(d, ds, true);
	dc = mbc_hex_to_raw(dx, &dcs);
	ASSERT_DATA(e, ds, dc, dcs);

	free(d);
	free(dc);
	free(dx);
	free(e);
}

CTEST(libmbc, convert__hex_to_raw_to_hex__lower) {
	uint8_t *dr;
	char *dx, *dxc, *ex;
	size_t dxs, drs;

	dxs = 0;
	dx = generate_random_data(&ex, &dxs, true, false);

	dr = mbc_hex_to_raw(dx, &drs);
	dxc = mbc_raw_to_hex(dr, drs, false);
	ASSERT_STR(ex, dxc);

	free(dx);
	free(dxc);
	free(dr);
	free(ex);
}

CTEST(libmbc, convert__hex_to_raw_to_hex__upper) {
	uint8_t *dr;
	char *dx, *dxc, *ex;
	size_t dxs, drs;

	dxs = 0;
	dx = generate_random_data(&ex, &dxs, true, true);

	dr = mbc_hex_to_raw(dx, &drs);
	dxc = mbc_raw_to_hex(dr, drs, true);
	ASSERT_STR(ex, dxc);

	free(dx);
	free(dxc);
	free(dr);
	free(ex);
}
