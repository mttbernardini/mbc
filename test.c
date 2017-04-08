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


	for (i = 0; i < user_data_len; i++)
		printf("%2x ", user_data[i]);
	printf("\n");

	mbc_set_user_key(user_key, user_key_len);
	encoded = mbc_encode(user_data, user_data_len);

	for (i = 0; i < user_data_len; i++)
		printf("%2x ", encoded[i]);
	printf("\n");

	decoded = mbc_decode(encoded, user_data_len);

	for (i = 0; i < user_data_len; i++)
		printf("%2x ", decoded[i]);
	printf("\n");

	mbc_free();

	return 0;
}
