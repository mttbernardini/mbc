/*
 * Copyright (c) 2017 Matteo Bernardini
 * Copyright (c) 2017 Marco Bonelli
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <getopt.h>
#include <string.h>
#include <libmbc.h>

#define USAGE_FORMAT   "Usage: %s %s\n"
#define VERSION_FORMAT "mbc %s\n%s"

#define print_version()  fprintf(stderr, VERSION_FORMAT, MBC_VERSION, VERSION_INFO)
#define print_usage()    fprintf(stderr, USAGE_FORMAT, CLI_NAME, USAGE_INFO)
#define print_help()     fprintf(stderr, VERSION_FORMAT"\n%s\n"USAGE_FORMAT"\n%s", MBC_VERSION, VERSION_INFO, SHORT_DESC, CLI_NAME, USAGE_INFO, HELP_MSG)

extern const char* MBC_VERSION;
static const char* VERSION_INFO = "Copyright (c) 2017 Matteo Bernardini & Marco Bonelli.\n";
static const char* USAGE_INFO = "[-xunvh] (-e | -d) -k <key>";
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
	" -u        When encoding and using -x, output uppercase characters.\n"
	" -n        When encoding and using -x, do not output the trailing newline.\n"
	"           NB: the presence of the newline does not affect decoding.\n"
	" -v        Shows program version and exits.\n"
	" -h        Shows this help message and exits.\n";

static const size_t RAW_CHUNK_SIZE = 32 << 20;  // TODO: make chunk size user editable,
static const size_t HEX_CHUNK_SIZE = 64 << 20;  //       but how do we deal with linearity?

static char* CLI_NAME;

void hex_codec(bool, bool, bool);
void raw_codec(bool);

int main(int argc, char* argv[]) {
	enum {NONE, ENCODE, DECODE, INVALID} mode;
	char opt, *key;
	bool hex, upper, trail_nl;

	CLI_NAME = argv[0];
	key      = NULL;
	hex      = false;
	upper    = false;
	trail_nl = true;
	mode     = NONE;

	while ((opt = getopt(argc, argv, "edk:xunvh")) != -1) {
		switch (opt) {
			case 'e':
				mode = (mode == NONE ? ENCODE : INVALID);
				break;

			case 'd':
				mode = (mode == NONE ? DECODE : INVALID);
				break;

			case 'k':
				key = optarg;
				break;

			case 'x':
				hex = true;
				break;

			case 'u':
				upper = true;
				break;

			case 'n':
				trail_nl = false;
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

	if (mode == NONE || mode == INVALID || key == NULL) {
		fprintf(stderr, "%s: please set a valid mode (-d OR -e) and a key (-k).\n", CLI_NAME);
		print_usage();
		return 1;
	}

	mbc_set_user_key((uint8_t*) key, strlen(key));
	atexit(mbc_free);

	if (hex)
		hex_codec(mode == ENCODE, upper, trail_nl);
	else
		raw_codec(mode == ENCODE);

	return 0;
}

void hex_codec(bool enc_mode, bool uppercase, bool trailing_newline) {
	uint8_t *raw_buffer_in, *raw_buffer_out;
	char    *hex_buffer_in, *hex_buffer_out;
	size_t  buffer_size_in, buffer_size_out;

	if (enc_mode) {

		raw_buffer_in = malloc(RAW_CHUNK_SIZE);
		if (raw_buffer_in == NULL)
			exit(1);

		while ((buffer_size_in = fread(raw_buffer_in, 1, RAW_CHUNK_SIZE, stdin))) {
			hex_buffer_out = mbc_encode_to_hex(raw_buffer_in, buffer_size_in, uppercase);
			if (hex_buffer_out == NULL) {
				free(raw_buffer_in);
				exit(1);
			}

			fwrite(hex_buffer_out, 1, buffer_size_in * 2, stdout);
			free(hex_buffer_out);
		}

		if (trailing_newline)
			fputs("\n", stdout);

		free(raw_buffer_in);
	} else {

		hex_buffer_in = malloc(HEX_CHUNK_SIZE + 1);
		if (hex_buffer_in == NULL)
			exit(1);

		while ((buffer_size_in = fread(hex_buffer_in, 1, HEX_CHUNK_SIZE, stdin))) {
			hex_buffer_in[buffer_size_in] = '\0';
			raw_buffer_out = mbc_decode_from_hex(hex_buffer_in, &buffer_size_out);
			if (raw_buffer_out == NULL) {
				free(hex_buffer_in);
				exit(1);
			}

			fwrite(raw_buffer_out, 1, buffer_size_out, stdout);
			free(raw_buffer_out);
		}

		free(hex_buffer_in);
	}
}

void raw_codec(bool enc_mode) {
	uint8_t *buffer;
	size_t buffer_size;
	void (*codec)(uint8_t*, size_t);

	buffer = malloc(RAW_CHUNK_SIZE);
	if (buffer == NULL)
		exit(1);

	codec = enc_mode ? mbc_encode_inplace : mbc_decode_inplace;

	while ((buffer_size = fread(buffer, 1, RAW_CHUNK_SIZE, stdin))) {
		codec(buffer, buffer_size);
		fwrite(buffer, 1, buffer_size, stdout);
	}

	free(buffer);
}
