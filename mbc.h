#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <stdbool.h>

/***********************
 **      PRIVATE      **
 ***********************/

enum mbc_options;

static uint8_t* user_key;
static size_t user_key_len;
static uint8_t* oct_key;
static size_t oct_key_len;
static enum mbc_options user_options;

/**
 * Generates the octal key (to be used in misc phase) from the user key.
 * @pre  Global `user_key` contains a valid user key.
 * @post Global `oct_key` is now the octal key.
 */
static bool make_oct_key();


/**
 * Converts raw data into an hexadecimal string.
 * @ret  NULL-terminated hexadecimal string.
 * @pre  `raw` contains raw bytes, `raw_size` is the size of `raw` and should be > 0.
 * @post The length of the returned string is even, containing only lower/uppercase (accordingly to `uppercase`) hexadecimal ASCII characters.
 */
static char* raw_to_hex(const uint8_t* raw, size_t raw_size, bool uppercase);


/**
 * Converts hexadecimal string into raw data.
 * @ret  Raw data array.
 * @pre  `hex` is a NULL-terminated string containing only lower/uppercase hexadecimal ASCII characters, and its length should be even and > 0.
 * @post `*raw_size_ptr` contains the size of the raw data returned.
 */
static uint8_t* hex_to_raw(const char* hex, size_t* raw_size_ptr);


/***********************
 **       PUBLIC      **
 ***********************/

enum mbc_options {
	MBC_ENCODE_HEX_IN  = 0x1,
	MBC_ENCODE_HEX_OUT = 0x2,
	MBC_DECODE_HEX_IN  = 0x4,
	MBC_DECODE_HEX_OUT = 0x8
};


/**
 * Sets the value of global `user_key` and calculates global `oct_key`.
 * @ret `true` if success, `false` otherwise.
 */
bool mbc_set_user_key(const uint8_t* key, size_t key_size);


/**
 * Sets The options for encoding/decoding.
 * @ret `true` if success, `false` otherwise (e.g. incompatible options).
 * @pre `mbc_options` is the sum of the desired options to apply.
 */
bool mbc_set_options(enum mbc_options options);


/**
 * Encodes data.
 * @ret  Encoded data.
 * @pre  `data` is casted to `char*` if option `MBC_ENCODE_HEX_IN` is set, otherwise `uint8_t*`.
 * @post Encoded data is returned accordingly to global `user_options` set.
 *       `*out_data_size` is only considered in case of hexadecimal input and raw output,
 *       and thus should be `NULL` in any other case.
 */
void* mbc_encode(void* data, size_t data_size, size_t* out_data_size);


/**
 * Decodes data.
 * @ret  Decoded data.
 * @pre  `data` is casted to `char*` if option `MBC_DECODE_HEX_IN` is set, otherwise `uint8_t*`.
 * @post Decoded data is returned accordingly to `user_options` set:
 *       `*out_data_size` is only considered for hexadecimal input and raw output,
 *       and thus should be `NULL` in any other case.
 */
void* mbc_decode(void* data, size_t data_size, size_t* out_data_size);


/**
 * Frees any dynamically allocated variables (basically the global keys).
 */
void mbc_free();
