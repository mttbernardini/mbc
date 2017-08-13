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

static void random_test_data(uint8_t** key, void* data, void* expected, size_t* key_size, size_t* data_size, bool hex, bool uppercase) {
	size_t i;

	if (*key_size == 0)
		*key_size = (size_t)(rand() % (MAX_KEY_SIZE - MIN_KEY_SIZE) + MIN_KEY_SIZE + 1);

	if (*data_size == 0)
		*data_size = (size_t)(rand() % (MAX_RAW_DATA_SIZE - MIN_RAW_DATA_SIZE) + MIN_RAW_DATA_SIZE + 1);

	CTEST_LOG("key: %zuB; data (%s): %zuB.", *key_size, hex ? (uppercase ? "HEX" : "hex") : "raw", *data_size);

	*key = malloc(*key_size);
	for (i = 0; i < *key_size; i++)
		(*key)[i] = (uint8_t)(rand() % 0xFF);

	if (hex) {
		*(char**)data     = malloc(*data_size + 1);

		if (uppercase)
			for (i = 0; i < *data_size; i++)
				(*(char**)data)[i] = HEX_CHARS_UPPER[rand() % 0x10];
		else
			for (i = 0; i < *data_size; i++)
				(*(char**)data)[i] = HEX_CHARS_LOWER[rand() % 0x10];

		(*(char**)data)[*data_size] = '\0';

		if (*data_size % 2 == 0) {
			*(char**)expected = malloc(*data_size + 1);
			memcpy(*(char**)expected, *(char**)data, *data_size + 1);
		} else {
			*(char**)expected = malloc(*data_size);
			memcpy(*(char**)expected, *(char**)data, *data_size - 1);
			(*(char**)expected)[*data_size] = '\0';
		}
	} else {
		*(uint8_t**)data     = malloc(*data_size);
		*(uint8_t**)expected = malloc(*data_size);

		for (i = 0; i < *data_size; i++)
			(*(uint8_t**)data)[i] = (uint8_t)(rand() % 0xFF);

		memcpy(*(uint8_t**)expected, *(uint8_t**)data, *data_size);
	}
}

CTEST(libmbc, encode_decode__inplace) {
	uint8_t *k, *d, *e;
	size_t ks, ds;

	ks = 0; ds = 0;
	random_test_data(&k, &d, &e, &ks, &ds, false, false);

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
	random_test_data(&k, &d, &e, &ks, &ds, false, false);

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
	random_test_data(&k, &d, &e, &ks, &ds, false, false);

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
	random_test_data(&k, &d, &e, &ks, &ds, false, false);

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
	uint8_t *k, *d, *dc, *e;
	char *dx;
	size_t ks, ds, dcs;

	ks = 0; ds = 0;
	random_test_data(&k, &d, &e, &ks, &ds, false, false);

	dx = mbc_raw_to_hex(d, ds, false);
	dc = mbc_hex_to_raw(dx, &dcs);
	ASSERT_DATA(e, ds, dc, dcs);

	free(k);
	free(d);
	free(dc);
	free(dx);
	free(e);
}

CTEST(libmbc, convert__raw_to_hex_to_raw__upper) {
	uint8_t *k, *d, *dc, *e;
	char *dx;
	size_t ks, ds, dcs;

	ks = 0; ds = 0;
	random_test_data(&k, &d, &e, &ks, &ds, false, false);

	dx = mbc_raw_to_hex(d, ds, true);
	dc = mbc_hex_to_raw(dx, &dcs);
	ASSERT_DATA(e, ds, dc, dcs);

	free(k);
	free(d);
	free(dc);
	free(dx);
	free(e);
}

CTEST(libmbc, convert__hex_to_raw_to_hex__lower) {
	uint8_t *k, *dr;
	char *dx, *dxc, *ex;
	size_t ks, dxs, drs;

	ks = 0; dxs = 0;
	random_test_data(&k, &dx, &ex, &ks, &dxs, true, false);

	dr = mbc_hex_to_raw(dx, &drs);
	dxc = mbc_raw_to_hex(dr, drs, false);
	ASSERT_STR(ex, dxc);

	free(k);
	free(dx);
	free(dxc);
	free(dr);
	free(ex);
}

CTEST(libmbc, convert__hex_to_raw_to_hex__upper) {
	uint8_t *k, *dr;
	char *dx, *dxc, *ex;
	size_t ks, dxs, drs;

	ks = 0; dxs = 0;
	random_test_data(&k, &dx, &ex, &ks, &dxs, true, true);

	dr = mbc_hex_to_raw(dx, &drs);
	dxc = mbc_raw_to_hex(dr, drs, true);
	ASSERT_STR(ex, dxc);

	free(k);
	free(dx);
	free(dxc);
	free(dr);
	free(ex);
}
