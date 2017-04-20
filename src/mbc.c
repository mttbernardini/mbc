#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <getopt.h>
#include <string.h>
#include <libmbc.h>

#define MBC_VERSION "0.1"

static const char* VERSION_INFO = "mbc " MBC_VERSION "\nCopyright (c) 2017 Matteo Bernardini & Marco Bonelli.\n";
static const char* USAGE_INFO = "[-xvh] (-e | -d) -k <key>";
static const char* SHORT_DESC =
	"mbc is a quick tool for encoding/decoding data via stdio using libmbc,\n"
	"a C implementation of the Mattyw & MeBeiM symmetric encryption algorithm.\n"
	"Source code available at https://github.com/mttbernardini/mbc.\n";
static const char* HELP_MSG =
	"Options:\n"
	" -e        Encodes data from stdio and outputs it to stdout.\n"
	" -d        Decodes data from stdio and outputs it to stdout.\n"
	" -k <key>  Sets the key for the encryption.\n"
	" -x        When encoding takes raw input from stdin and outputs encoded\n"
	"           data as an hexadecimal string to stdout. When decoding takes\n"
	"           an hexadecimal string representing encoded data from stdin\n"
	"           and outputs raw decoded data to stdout.\n"
	" -v        Shows program version and exits.\n"
	" -h        Shows this help message and exits.\n";

static const size_t RAW_CHUNK_SIZE = 32 << 20;  // TODO: make chunk size user editable,
static const size_t HEX_CHUNK_SIZE = 64 << 20;  //       but how do we deal with linearity?

static char* CLI_NAME;

void print_version(void);
void print_usage(void);
void print_help(void);
void print_invalid(void);
int core(bool enc_mode, bool hex_mode);

int main(int argc, char* argv[]) {
	int ret;
	char opt, *key;
	size_t key_size;
	bool enc, hex, mode_set;

	CLI_NAME = argv[0];

	ret      = 0;
	key      = NULL;
	hex      = false;
	mode_set = false;

	while ((opt = getopt(argc, argv, "edk:xvh")) != -1) {
		switch (opt) {
			case 'e':
				if (!mode_set) {
					enc = true;
					mode_set = true;
				} else {
					print_invalid();
					return 1;
				}
				break;

			case 'd':
				if (!mode_set) {
					enc = false;
					mode_set = true;
				} else {
					print_invalid();
					return 1;
				}
				break;

			case 'k':
				key = optarg;
				break;

			case 'x':
				hex = true;
				break;

			case 'h':
				print_help();
				return 0;

			case 'v':
				print_version();
				return 0;

			default:
				print_usage();
				return 1;
		}
	}

	if (!mode_set || key == NULL) {
		print_invalid();
		return 1;
	}

	key_size = strlen(key);
	if(mbc_set_user_key((uint8_t*) key, key_size)) {
		ret = core(enc, hex);
		mbc_free();
	}

	return ret;
}

void print_version(void) {
	fprintf(stderr, "%s", VERSION_INFO);
}

void print_usage(void) {
	fprintf(stderr, "Usage: %s %s\n", CLI_NAME, USAGE_INFO);
}

void print_help(void) {
	print_version();
	fprintf(stderr, "\n%s\n", SHORT_DESC);
	print_usage();
	fprintf(stderr, "\n%s", HELP_MSG);
}

void print_invalid(void) {
	fprintf(stderr, "Please set a valid mode (-d OR -e) and a key (-k).\n");
	print_usage();
}

int core(bool enc_mode, bool hex_mode) {
	uint8_t *buffer_in_raw, *buffer_out_raw;
	char *buffer_in_hex, *buffer_out_hex;
	size_t bytes_read, bytes_to_write;

	if (hex_mode) {
		if (enc_mode) {

			buffer_in_raw = malloc(RAW_CHUNK_SIZE);
			if (buffer_in_raw == NULL)
				return 1;

			while ((bytes_read = fread(buffer_in_raw, 1, RAW_CHUNK_SIZE, stdin))) {
				buffer_out_hex = mbc_encode_to_hex(buffer_in_raw, bytes_read, false);
				if (buffer_out_hex == NULL) {
					free(buffer_in_raw);
					return 1;
				}

				fwrite(buffer_out_hex, 1, bytes_read * 2, stdout);
				free(buffer_out_hex);
			}

			free(buffer_in_raw);
		} else {

			buffer_in_hex = malloc(HEX_CHUNK_SIZE + 1);
			if (buffer_in_hex == NULL)
				return 1;

			buffer_in_hex[HEX_CHUNK_SIZE] = '\0';

			while ((bytes_read = fread(buffer_in_hex, 1, HEX_CHUNK_SIZE, stdin))) {
				buffer_out_raw = mbc_decode_from_hex(buffer_in_hex, &bytes_to_write);
				if (buffer_out_raw == NULL) {
					free(buffer_in_hex);
					return 1;
				}

				fwrite(buffer_out_raw, 1, bytes_to_write, stdout);
				free(buffer_out_raw);
			}

			free(buffer_in_hex);
		}
	} else {

		buffer_in_raw = malloc(RAW_CHUNK_SIZE);
		if (buffer_in_raw == NULL)
			return 1;

		if (enc_mode) {
			while ((bytes_read = fread(buffer_in_raw, 1, RAW_CHUNK_SIZE, stdin))) {
				mbc_encode_inplace(buffer_in_raw, bytes_read);
				fwrite(buffer_in_raw, 1, bytes_read, stdout);
			}
		} else {
			while ((bytes_read = fread(buffer_in_raw, 1, RAW_CHUNK_SIZE, stdin))) {
				mbc_decode_inplace(buffer_in_raw, bytes_read);
				fwrite(buffer_in_raw, 1, bytes_read, stdout);
			}
		}

		free(buffer_in_raw);
	}

	return 0;
}
