#include <stdio.h>
#include <stdbool.h>
#include <inttypes.h>
#include <stdlib.h>
#include <string.h>
#include <libmbc.h>

int main(int argc, char* argv[]) {
	uint8_t *encoded, *decoded, *user_data, *user_key;
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
		printf("%02x ", user_data[i]);
	printf("\n");

	for (i = 0; i < user_data_len; i++)
		printf("%02x ", encoded[i]);
	printf("\n");

	for (i = 0; i < user_data_len; i++)
		printf("%02x ", decoded[i]);
	printf("\n");

	free(encoded);
	free(decoded);
	mbc_free();

	return 0;
}
