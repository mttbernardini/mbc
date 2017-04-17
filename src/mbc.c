#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <getopt.h>
#include <string.h>
#include <libmbc.h>

static const char* CLI_VERSION = "0.1";
static char* cli_name; // will be set by main()
static const char* USAGE_OPT =
	"(-e | -d) -k key [-x] [-v] [-h]";
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

static const int CHUNK_FACTOR = 32 << 20;

void mbc_core(bool do_enc, const char* user_key, bool hex_mode) {
	uint8_t *buffer, *buffer_out;
	char *buffer_hex_out;
	size_t user_key_len, buffer_size, buffer_out_size, chunk_n, chunk_size;

	// Set key
	user_key_len = strlen(user_key);
	mbc_set_user_key((uint8_t*)user_key, user_key_len);

	chunk_size = user_key_len * CHUNK_FACTOR;  //FIXME: this method is not 100% linear (fails on the last chunk)

	buffer  = malloc(chunk_size);  //FIXME: handle malloc error
	chunk_n = 0;

	if (hex_mode && do_enc) {
		while((buffer_size = fread(buffer, 1, chunk_size, stdin))) {
			buffer_hex_out  = mbc_encode_to_hex(buffer, buffer_size, false);  //FIXME: handle NULL return
			buffer_out_size = strlen(buffer_hex_out);

			fwrite(buffer_hex_out, 1, buffer_out_size, stdout);
			free(buffer_hex_out);
			chunk_n++;
		}
	}

	else if (hex_mode && !do_enc) {
		while((buffer_size = fread(buffer, 1, chunk_size, stdin))) {
			buffer_out = mbc_decode_from_hex((char*)buffer, &buffer_out_size);  //FIXME: handle NULL return

			fwrite(buffer_out, 1, buffer_out_size, stdout);
			free(buffer_out);
			chunk_n++;
		}
	}

	else {
		while ((buffer_size = fread(buffer, 1, chunk_size, stdin))) {
			if(do_enc)
				mbc_encode_inplace(buffer, buffer_size);
			else
				mbc_decode_inplace(buffer, buffer_size);

			fwrite(buffer, 1, buffer_size, stdout);
			chunk_n++;
		}
	}

	mbc_free();
	free(buffer);
}

void print_version() {
	fprintf(stderr, "%s %s\n", cli_name, CLI_VERSION);
}

void print_usage() {
	fprintf(stderr, "Usage: %s %s\n", cli_name, USAGE_OPT);
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

	cli_name = argv[0];

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

	fprintf(stderr, "debug: Will encode? %d, hex? %d, key? %s\n\n", enc_flag, hex_mode, key);

	mbc_core(enc_flag, key, hex_mode);

	return 0;

}
