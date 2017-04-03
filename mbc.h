#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <stdbool.h>

/**
 * -- SEMANTICS --
 * char*      NULL terminated string (array of char)
 * uint8_t*   array of raw bytes (should provide length)
 * 
 * TODO: Is it a good idea to use typedefs for semantic types?
 * e.g. string for strings or swap_map for '-xxx-yyy' pattern bytes
 * 
 */


/**
 * -- CONVENTIONS --
 * size_t is used for counters (caution when reverse looping, size_t is unsigned)
 * char* is used for null-terminated strings (not uint8_t*)
 * stdint.h types are preferred
 * 
 */

 
/**
 * -- USAGE --
 * Gotta work on that...
 * Probably something like:
 *    mbc_set_options(...);
 *    mbc_set_user_key(...);
 *    mbc_encode(...); / mbc_decode(...);
 *    mbc_reset(...);
 *    mbc_encode(...); / mbc_decode(...);
 *    mbc_reset(...);
 *    mbc_encode(...); / mbc_decode(...);
 *    ...
 *    mbc_free(...);
 */


/* ##### PROTOTIPES BEGIN HERE ##### */

/* PRIVATE */

static char* user_key;
static size_t user_key_len;
static uint8_t* hex_key;
static size_t hex_key_len;
static uint8_t* oct_key;
static size_t oct_key_len;

/* TODO: Structs for keys? We'll probably need this.
 * Something like: 
 *     struct {
 *         char* key;
 *         char* cur_pos;
 *         size_t len;
 *     } user_key;
 */

static struct {
	bool encode_hex_input,
	     encode_hex_output,
	     decode_hex_input,
	     decode_hex_output;
} options;

// TODO: Do we really need all these options?

/**
 * Generates the octal key to be used in misc phase from the user key.
 * @ret  okey is the new octal key
 * @pre  key length > 0
 * @post okey contains swap-map bytes in format '-xxx-yyy', *oct_key_size_p contains the size of okey
 */
static uint8_t* make_oct_key(const char* key, size_t* okey_size_p);


/**
 * XORes the user key against itself if it's longer than max_key_size
 * @ret  fkey is the new key
 * @pre  key length > 0, max_key_size > 0
 * @post (*fkey_size_p = size of fkey) <= max_key_size
 */
static uint8_t* fit_hex_key(const char* key, size_t* fkey_size_p, size_t max_key_size);


/**
 * Converts raw data into an hexadecimal string.
 * @ret  hex string
 * @pre  raw contains raw bytes, raw_size is the size of raw and should be > 0
 * @post hex length is even, containing only hex chars (uppercase/lowercase?)
 */
static char* raw_to_hex(const uint8_t* raw, size_t raw_size);


/**
 * Converts hexadecimal string into raw data.
 * @ret  raw data array
 * @pre  hex length should be even and > 0, containing valid lower/uppercase hex digits
 * @post *raw_size_p contains the size of raw
 */
static int8_t* hex_to_raw(const char* hex, size_t* raw_size_p);


// NB: the following functions need wrapper functions which take care of calculating the oct_key
/**
 * Encodes/decodes an array of raw bytes, using 2 keys.
 * @pre  data_size > 0, xkey length <= data_size (use fit_hex_key before, otherwhise overflow bytes are ignored)
 * @post data is encoded/decoded
 */
void encoder(uint8_t* data, const char* xkey, const uint8_t* okey, size_t data_size, size_t okey_size);
void decoder(uint8_t* data, const char* xkey, const uint8_t* okey, size_t data_size, size_t okey_size);



/* PUBLIC */

/**
 * Sets the value of user_key and calculates oct_key and hex_key.
 * @ret true if success, false otherwise
 */
bool mbc_set_user_key(char* key, size_t len);
/* ^ TODO: maybe allow for raw data user key too (uint8_t)? */
// issue: hex_key cannot be calculated if data size is unknown (how to handle cycling?)

/**
 * Sets the options for encoding/decoding.
 * @ret true if success, false otherwise (e.g. incompatible options)
 */
bool mbc_set_options(bool encode_hex_input, bool encode_hex_output, bool decode_hex_input, bool decode_hex_output);
 // do really C libraries set options like this? how interesting

/**
 * Encode data.
 * @ret encoded data (as char* if options.encode_hex_output, as uint8_t* otherwise)
 * @pre data is casted to char* if options.encode_hex_input, otherwise uint8_t*
 */
void* mbc_encode(void* data, size_t len);

/**
 * Encode data.
 * @ret encoded data (as char* if options.decode_hex_output, as uint8_t* otherwise)
 * @pre data is casted to char* if options.decode_hex_input, otherwise uint8_t*
 */
void* mbc_decode(void* data, size_t len);

/**
 * Resets the algorithm, to be used when the data to encode/decode is finished
 * new data needs to be encoded/decoded.
 * @post global keys positions are reset
 */
void mbc_reset();

/**
 * Frees any dinamically allocated variable (e.g. global keys)
 */
void mbc_free();
