#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdint.h>
#include <string.h>

#define DEBUG_ON 0

#define BYTE_TO_BINARY_PATTERN "%c%c%c%c%c%c%c%c"
#define BYTE_TO_BINARY(byte)\
	(byte & 0x80 ? '1' : '0'),\
	(byte & 0x40 ? '1' : '0'),\
	(byte & 0x20 ? '1' : '0'),\
	(byte & 0x10 ? '1' : '0'),\
	(byte & 0x08 ? '1' : '0'),\
	(byte & 0x04 ? '1' : '0'),\
	(byte & 0x02 ? '1' : '0'),\
	(byte & 0x01 ? '1' : '0')

const uint32_t CHUNK_SIZE = 32 << 10;  // split data in chunks of 32kiB

// wait a minute, do we really need this? what about fprintf(stdout, "%x", data) or something similar?
char* raw_to_hex(const uint8_t* raw, size_t raw_size) {
	/**
	 * Converts raw data into an hexadecimal string.
	 * @ret  `hex` string
	 * @pre  `raw` contains raw bytes, `raw_size` > 0 (malloc() undef behavior otherwhise)
	 * @post `hex` length is even, containing only uppercase hex chars (see map)
	 */

	uint8_t* hex;
	register size_t i, j;

	const uint8_t hex_map[16] = {
		'0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'A', 'B', 'C', 'D', 'E', 'F'
	};

	hex = malloc(raw_size*2 + 1);
	hex[raw_size*2] = '\0';

	for (i = 0, j = 0; i < raw_size; i++, j += 2) {
		hex[j]	 = hex_map[(raw[i] >> 4) & 0x0F];
		hex[j+1] = hex_map[ raw[i]	     & 0x0F];
	}

	return hex;
}


uint8_t* hex_to_raw(const char* hex, size_t* raw_size_p) {
	/**
	 * Converts hexadecimal string into raw data.
	 * @ret  `raw` data array
	 * @pre  `hex` length should be even and > 0, containing valid lower/uppercase hex digits
	 * @post `*raw_size_p` contains the size of `raw`
	 */

	// might TODO: don't require even length, add leading 0

	uint8_t* raw;
	size_t hex_size;
	register size_t i, shift;

	hex_size	= strlen(hex);
	*raw_size_p = hex_size/2;
	raw	        = calloc(1, *raw_size_p);

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


uint8_t* make_oct_key(const char* key, size_t* okey_size_p) {
	/**
	 * Generates the octal key to be used in misc phase from the encryption key.
	 * @ret  `okey` is the new octal key
	 * @pre  `key` length > 0 (otherwhise malloc() is undef behavior)
	 * @post `okey` contains swap-map bytes in format -xxx-yyy, `*okey_size_p` is the size of `okey`
	 */

	size_t key_size;
	register size_t i;
	uint8_t* okey;

	key_size     = strlen(key);
	*okey_size_p = key_size;
	okey         = malloc(key_size);

	for (i = 0; i < key_size; i++) {
		okey[i] = (key[i] & 0x70) + ((key[i] >> 1) & 0x07);
		if (key[i] & 0x1) okey[i] = ~okey[i];
	}

	/****

	-- RATIONALE --

	EXTRACT KEY BYTE:
	key[i]            : -xxxyyyL >> 1 =
	                    0-xxxyyy &
	key mask 1 (0x07) : 00000111 =
	okey[i]           = 00000yyy

	key[i]            : -xxxyyyL &
	key mask 1 (0x70) : 01110000 =
	okey[i]          += 0xxx0000 =
	okey[i]           : 0xxx0yyy

	IF key[i] & 0x1:
	okey[i] = ~okey[i];

	IF okey[i] == 0:
	okey[i] = 10000000

	****/

	return okey;
}


char* fit_hex_key(const char* key, size_t max_key_size) {
	/**
	 * XORES the key against itself if it's longer than `max_key_size`
	 * @ret  `fkey` is the new key
	 * @pre  `key` length > 0, `max_key_size` > 0
	 * @post `fkey` length <= `max_key_size`
	 */

	size_t key_size;
	register size_t i;
	char* fkey;

	key_size = strlen(key);

	if (key_size > max_key_size) {
		fkey = malloc(max_key_size + 1);
		fkey[max_key_size] = '\0'; // NULL terminator
		memcpy(fkey, key, max_key_size);

		for (i = max_key_size; i < max_key_size; i++)
			fkey[i % max_key_size] ^= key[i];

	} else {
		fkey = malloc(key_size + 1);
		fkey[key_size] = '\0'; // NULL terminator
		memcpy(fkey, key, key_size);
	}

	return fkey;

}


void encoder(uint8_t* data, const char* xkey, const uint8_t* okey, size_t data_size, size_t okey_size) {
	/**
	 * Encodes an array of raw bytes, using 2 keys.
	 * @pre  `data_size` > 0, `xkey` length <= `data_size` (use fit_hex_key before, otherwhise overflow bytes are ignored)
	 * @post `data` is encoded
	 */

	size_t xkey_size;
	register size_t i, j;
	uint8_t l_bit, r_bit, mask;

	xkey_size = strlen(xkey);

	#if DEBUG_ON>0
		fprintf(stderr, "%s    input data\n", raw_to_hex(data, data_size));
	#endif

	// XOR PART
	for (i = 0; i < data_size; i++) {
			data[i] ^= xkey[i % xkey_size];
	}

	#if DEBUG_ON==1
		fprintf(stderr, "%s    after XOR\n", raw_to_hex(data, data_size));
	#endif

	// MISC PART
	for (i = 0; i < data_size; i++) {
		for (j = 0; j < okey_size; j++) {
			l_bit = (okey[j] & 0x70) >> 4;
			r_bit = (okey[j] & 0x07);

			mask = (0x1 << l_bit) ^ (0x1 << r_bit);

			#if DEBUG_ON==2
				fprintf(stderr, BYTE_TO_BINARY_PATTERN"  input (*)\n"BYTE_TO_BINARY_PATTERN"  mask =\n", BYTE_TO_BINARY(data[i]), BYTE_TO_BINARY(mask));
			#endif

			if (((data[i] >> l_bit) & 0x1) != ((data[i] >> r_bit) & 0x1))
				data[i] ^= mask;

				#if DEBUG_ON==2
					fprintf(stderr, BYTE_TO_BINARY_PATTERN"  output\n\n", BYTE_TO_BINARY(data[i]));
				#endif
		}
	}

	#if DEBUG_ON>0
		fprintf(stderr, "%s    after MISC\n", raw_to_hex(data, data_size));
	#endif

}


void decoder(uint8_t* data, const char* xkey, const uint8_t* okey, size_t data_size, size_t okey_size) {
	/**
	 * Decodes an array of raw bytes, using 2 keys.
	 * @pre  `data_size` > 0, `xkey` length <= `data_size` (use fit_hex_key before, otherwhise overflow bytes are ignored)
	 * @post `data` is decoded
	 */

	size_t xkey_size;
	register size_t i, j;
	uint8_t l_bit, r_bit, mask;

	xkey_size = strlen(xkey);

	#if DEBUG_ON>0
		fprintf(stderr, "%s    input data\n", raw_to_hex(data, data_size));
	#endif

	// MISC PART
	for (i = 0; i < data_size; i++) {
		for (j = okey_size-1; j < okey_size; j--) {
			l_bit = (okey[j] & 0x70) >> 4;
			r_bit = (okey[j] & 0x07);

			mask = (0x1 << l_bit) ^ (0x1 << r_bit);

			#if DEBUG_ON==2
				fprintf(stderr, BYTE_TO_BINARY_PATTERN"  input (*)\n"BYTE_TO_BINARY_PATTERN"  mask =\n", BYTE_TO_BINARY(data[i]), BYTE_TO_BINARY(mask));
			#endif

			if (((data[i] >> l_bit) & 0x1) != ((data[i] >> r_bit) & 0x1))
				data[i] ^= mask;

			#if DEBUG_ON==2
				fprintf(stderr, BYTE_TO_BINARY_PATTERN"  output\n\n", BYTE_TO_BINARY(data[i]));
			#endif
		}
	}

	#if DEBUG_ON>0
		fprintf(stderr, "%s    after reverse MISC\n", raw_to_hex(data, data_size));
	#endif

	// XOR PART
	for (i = 0; i < data_size; i++) {
		data[i] ^= xkey[i % xkey_size];
	}

	#if DEBUG_ON==1
		fprintf(stderr, "%s    after XOR\n", raw_to_hex(data, data_size));
	#endif

}


void mbc_core(bool do_enc, const char* user_key, bool hex_mode) {
	/**
	 * Main function with common parts, chunk division and key preparations
	 * @ret TODO: return something
	 */

	uint8_t *oct_key, *buffer;
	char* hex_key;
	size_t bytes_read, oct_key_size, chunk_n;

	// prepare oct key
	oct_key = make_oct_key(user_key, &oct_key_size);

	#if DEBUG_ON == 1
		fprintf(stderr, "Encoding? %d\n", do_enc);
		fprintf(stderr, "Octal key: %s\n", raw_to_hex(oct_key, strlen(oct_key)));
	#endif

	buffer  = malloc(CHUNK_SIZE);
	chunk_n = 0;

	while (bytes_read = fread(buffer, 1, CHUNK_SIZE, stdin)) {

		hex_key = fit_hex_key(user_key, bytes_read);

		#if DEBUG_ON == 1
			fprintf(stderr, "< chunk %d | %d bytes read >\n", chunk_n+1, bytes_read);
			fprintf(stderr, "Hex key:   %s\n", raw_to_hex(hex_key, strlen(hex_key)));
		#endif

		if(do_enc)
			encoder(buffer, hex_key, oct_key, bytes_read, oct_key_size);
		else
			decoder(buffer, hex_key, oct_key, bytes_read, oct_key_size);

		fwrite(buffer, 1, bytes_read, stdout);
		free(hex_key);

		chunk_n++;
	}

	free(oct_key);
	free(buffer);
}



/*** TEST ***/
int main(int argc, char* argv[]) {

	if (argc < 3) exit(1);

	int do_enc = (argv[1][0] == '1');

	mbc_core(do_enc, argv[2], 0);

	fprintf(stderr, "\n");

	return 0;

}

// ./test 1 ciao

/* TO EXPORT */
/*void mbc_encode(uint8_t* data, char* key, size_t data_size, bool hex_mode) {

}
void mbc_decode(uint8_t* data, char* key, size_t data_size, bool hex_mode) {

}*/
