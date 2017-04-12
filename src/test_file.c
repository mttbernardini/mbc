#include <stdio.h>
#include <string.h>
#include <libmbc.h>

int main(int argc, char* argv[]) {
	FILE *f;
	uint8_t data[1500];
	size_t data_size;

	f = fopen(argv[1], "rb");
	data_size = fread(data, 1, 1500, f);
	fclose(f);

	mbc_set_user_key(argv[2], strlen(argv[2]));
	mbc_encode_inplace(data, data_size);
	mbc_decode_inplace(data, data_size);

	f = fopen(argv[3], "wb");
	fwrite(data, 1, data_size, f);
	fclose(f);
	return 0;
}
