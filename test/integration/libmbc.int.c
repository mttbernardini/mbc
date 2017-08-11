#include <time.h>
#include <stdlib.h>
#include <stdint.h>
#include <ctest.h>
#include <libmbc.h>

static const size_t MAX_KEY_SIZE   = 1024L * 1024L * 32L;
static const size_t MAX_DUMMY_SIZE = 1024L * 1024L * 128L;

CTEST_DATA(libmbc__raw) {
	size_t key_size;
	size_t dummy_size;
	size_t expected_size;
	uint8_t* key;
	uint8_t* dummy;
	uint8_t* expected;
};

CTEST_SETUP(libmbc__raw) {
	size_t i;

	data->key_size      = (size_t)rand() % MAX_KEY_SIZE   + 1;
	data->dummy_size    = (size_t)rand() % MAX_DUMMY_SIZE + 1;
	data->expected_size = data->dummy_size;

	data->key      = malloc(sizeof(uint8_t) * data->key_size     );
	data->dummy    = malloc(sizeof(uint8_t) * data->dummy_size   );
	data->expected = malloc(sizeof(uint8_t) * data->expected_size);

	CTEST_LOG("Random key  size was: %lu.", data->key_size);
	CTEST_LOG("Random data size was: %lu.", data->dummy_size);

	for (i = 0; i < data->key_size; i++)
		data->key[i] = (size_t)rand() % 0xFF;

	for (i = 0; i < data->dummy_size; i++) {
		data->dummy[i]    = (size_t)rand() % 0xFF;
		data->expected[i] = data->dummy[i];
	}

	mbc_set_user_key(data->key, data->key_size);
}

CTEST_TEARDOWN(libmbc__raw) {
	free(data->key     );
	free(data->dummy   );
	free(data->expected);
}

/*
CTEST_DATA(libmbc__to_hex) {

};

CTEST_SETUP(libmbc__to_hex) {

}

CTEST_TEARDOWN(libmbc__to_hex) {

}

CTEST_DATA(libmbc__from_hex) {

};

CTEST_SETUP(libmbc__from_hex) {

}

CTEST_TEARDOWN(libmbc__from_hex) {

}
*/

CTEST2(libmbc__raw, encode_decode__inplace) {
	mbc_encode_inplace(data->dummy, data->dummy_size);
	mbc_decode_inplace(data->dummy, data->dummy_size);
	ASSERT_DATA(data->expected, data->expected_size, data->dummy, data->dummy_size);
}

CTEST2(libmbc__raw, encode_decode) {
	uint8_t *encoded, *decoded;

	encoded = mbc_encode(data->dummy, data->dummy_size);
	decoded = mbc_decode(encoded, data->dummy_size);
	ASSERT_DATA(data->expected, data->expected_size, decoded, data->dummy_size);
}
