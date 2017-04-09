#include <stdio.h>
#include <stdbool.h>
#include <inttypes.h>
#include <stdlib.h>
#include "mbc.c"
// ^^^^^^^^^^^^^ Never include the .c! This include is for quick testing purposes only.

int main(int argc, char* argv[]) {
	uint8_t *encoded, *decoded, *user_data;
	size_t user_data_len, user_key_len;
	register size_t i;

	user_data = argv[1];
	user_data_len = strlen(user_data);
	user_key = argv[2];
	user_key_len = strlen(user_key);

	mbc_set_user_key(user_key, user_key_len);
	encoded = mbc_encode(user_data, user_data_len);
	decoded = mbc_decode(encoded, user_data_len);

	printf("Testing encoding/decoding:\n");
	for (i = 0; i < user_data_len; i++)
		printf("%2x ", user_data[i]);
	printf("\n");

	for (i = 0; i < user_data_len; i++)
		printf("%2x ", encoded[i]);
	printf("\n");

	for (i = 0; i < user_data_len; i++)
		printf("%2x ", decoded[i]);
	printf("\n");

	uint8_t test_raw[] = {0x10, 0xff, 0xaa, 0x01, 0x11};
	char test_hex[] = "00fFAA0111";
	size_t test_raw_size, test_hex_size, raw_result_size;
	uint8_t* raw_result;
	char* hex_result;

	test_raw_size = sizeof(test_raw);
	test_hex_size = strlen(test_hex);

	hex_result = mbc_raw_to_hex(test_raw, test_raw_size, false);
	raw_result = mbc_hex_to_raw(test_hex, &raw_result_size);

	printf("\nTesting conversion:\n   Raw: ");
	for (i = 0; i < test_raw_size; i++)
		printf("%2x", test_raw[i]);
	printf("\n");

	printf("To hex: ");
	printf("%s\n", hex_result);

	printf("   Hex: ");
	printf("%s\n", test_hex);

	printf("To raw: ");
	for (i = 0; i < raw_result_size; i++)
		printf("%2x", raw_result[i]);
	printf("\n");

	mbc_free();
	free(raw_result);
	free(hex_result);

	return 0;
}
