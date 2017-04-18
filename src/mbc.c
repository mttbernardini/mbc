#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <getopt.h>
#include <string.h>
#include <libmbc.h>

static const char* CLI_VERSION = "0.1";
static char* CLI_NAME; // will be set by main()
static const char* USAGE_OPT = "(-e | -d) -k key [-x] [-v] [-h]";
static const char* HELP_MSG =
	"mbc is a quick tool for encoding/decoding data from stdio using the libmbc\n"
	"library, which uses the Mattyw&MeBeiM Symmetric Encryption Algorithm.\n\n"
	"OPTIONS:\n"
	" -e        Encodes data from stdio and outputs it to stdout.\n"
	" -d        Decodes data from stdio and outputs it to stdout.\n"
	" -k <key>  Sets the key for the encryption.\n"
	" -x        When encoding takes raw input from stdin and outputs encoded\n"
	"           data as an hexadecimal string to stdout. When decoding takes\n"
	"           an hexadecimal string representing encoded data from stdin\n"
	"           and outputs raw decoded data to stdout.\n"
	" -v        Shows program version and exits.\n"
	" -h        Shows this help message and exits.\n";

static const size_t RAW_CHUNK_SIZE = 32 << 20;
static const size_t HEX_CHUNK_SIZE = 64 << 20;

void mbc_core(bool do_enc, const char* user_key, bool hex_mode) {
	uint8_t *buffer_in_raw, *buffer_out_raw;
	char *buffer_in_hex, *buffer_out_hex;
	size_t user_key_len, bytes_read, bytes_to_write;

	user_key_len = strlen(user_key);
	mbc_set_user_key((uint8_t*) user_key, user_key_len);

	if (hex_mode) {
		if (do_enc) {

			buffer_in_raw = malloc(RAW_CHUNK_SIZE);  //TODO: handle malloc error

			while ((bytes_read = fread(buffer_in_raw, 1, RAW_CHUNK_SIZE, stdin))) {
				buffer_out_hex = mbc_encode_to_hex(buffer_in_raw, bytes_read, false);
				fwrite(buffer_out_hex, 1, bytes_read * 2, stdout);
				free(buffer_out_hex);
			}
		} else {

			buffer_in_hex = malloc(HEX_CHUNK_SIZE + 1);  //TODO: handle malloc error

			while ((bytes_read = fread(buffer_in_hex, 1, HEX_CHUNK_SIZE, stdin))) {
				buffer_in_hex[HEX_CHUNK_SIZE] = '\0';
				buffer_out_raw = mbc_decode_from_hex(buffer_in_hex, &bytes_to_write);
				fwrite(buffer_out_raw, 1, bytes_to_write, stdout);
				free(buffer_out_raw);
			}
		}
	} else {

		buffer_in_raw = malloc(RAW_CHUNK_SIZE);  //TODO: handle malloc error

		if (do_enc) {
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

	mbc_free();
}

void print_version() {
	fprintf(stderr, "%s %s\n", CLI_NAME, CLI_VERSION);
}

void print_usage() {
	fprintf(stderr, "Usage: %s %s\n", CLI_NAME, USAGE_OPT);
}

void print_help() {
	print_version();
	print_usage();
	fprintf(stderr, "\n%s", HELP_MSG);
}

void print_invalid() {
	fprintf(stderr, "Please set a valid mode (-d or -e) and a key (-k).\n");
	print_usage();
}

int main(int argc, char* argv[]) {
	char opt;
	char* key;
	bool enc_flag, dec_flag, mode_set, hex_mode;

	key = NULL;
	hex_mode = false;
	enc_flag = false;
	dec_flag = false;
	mode_set = false;

	CLI_NAME = argv[0];

	while ((opt = getopt(argc, argv, "edk:xvh")) != -1) {
		switch (opt) {
			case 'e':
				if (!dec_flag) {
					enc_flag = true;
					mode_set = true;
				}
				break;
			case 'd':
				if (!enc_flag) {
					dec_flag = true;
					mode_set = true;
				}
				break;
			case 'k':
				key = optarg;
				break;
			case 'x':
				hex_mode = true;
				break;
			case 'h':
				print_help();
				return 1;
			case 'v':
				print_version();
				return 1;
			default:
				print_usage();
				return 1;
		}
	}

	if (!mode_set || key == NULL) {
		print_invalid();
		return 1;
	}

	// fprintf(stderr, "debug: Will encode? %d, hex? %d, key? %s\n\n", enc_flag, hex_mode, key);

	mbc_core(enc_flag, key, hex_mode);

	return 0;
}
