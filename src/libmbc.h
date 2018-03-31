#ifndef LIBMBC_H_INCLUDED
#define LIBMBC_H_INCLUDED

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>

typedef struct mbc_token_s* mbc_token_t;

/**
 * Generates a struct containing encoding instructions for encode/decode functions.
 * @ret a pointer to a `mbc_token_t` struct. NULL if the struct cannot be allocated.
 * @pre `key` is an array of bytes; `key_size` > 0
 */
mbc_token_t mbc_generate_token(const uint8_t* key, size_t key_size);

/**
 * TODO: add doc
 */
void mbc_set_key_offset(mbc_token_t key, size_t chunk_size, size_t n_chunk);

/**
 * Frees key data.
 * @post `key` is an invalid pointer.
 */
void mbc_free_token(mbc_token_t key);

/**
 * Encodes data in place.
 * @pre  `key` is a valid generated token; `data` is an array of bytes; `data_size > 0`.
 * @post `data` contains encoded bytes.
 */
void mbc_encode_inplace(mbc_token_t key, uint8_t* data, size_t data_size);

/**
 * Decodes data in place.
 * @pre  `key` is a valid generated token; `data` is a dynamically allocated array of bytes; `data_size > 0`.
 * @post `data` contains decoded bytes.
 */
void mbc_decode_inplace(mbc_token_t key, uint8_t* data, size_t data_size);

/**
 * TODO: add doc
 */
void mbc_encode_chunk_inplace(mbc_token_t key, uint8_t* chunk, size_t chunk_size);

/**
 * Encodes data in a new array.
 * @ret  Encoded data as array, `NULL` if the array cannot be allocated.
 * @pre  `key` is a valid generated token; `data` is an array of bytes; `data_size > 0`.
 */
uint8_t* mbc_encode(mbc_token_t key, const uint8_t* data, size_t data_size);

/**
 * Decodes data in a new array.
 * @ret  Decoded data as array, `NULL` if the array cannot be allocated.
 * @pre  `key` is a valid generated token; `data` is an array of bytes; `data_size > 0`.
 */
uint8_t* mbc_decode(mbc_token_t key, const uint8_t* data, size_t data_size);

/**
 * Encodes data to hexadecimal output.
 * @ret  NUL-terminated hexadecimal string, `NULL` if the string cannot be allocated.
 * @pre  `key` is a valid generated token; `raw_in` is an array of bytes; `raw_size > 0`.
 * @post The length of the returned string is even, containing only lower/uppercase (accordingly to `uppercase`) hexadecimal ASCII characters.
 */
char* mbc_encode_to_hex(mbc_token_t key, const uint8_t* raw_in, size_t raw_size, bool uppercase);

/**
 * Decodes data from hexadecimal input.
 * @ret  Raw decoded data array, `NULL` if the array cannot be allocated.
 * @pre  `key` is a valid generated token;
 *       `hex_in` is a non-empty NUL-terminated string containing only lower/uppercase hexadecimal ASCII characters.
 *       If the length of the string is not even the last extra char is discarded.
 * @post `*decoded_size_ptr` contains the size of decoded data returned.
 */
uint8_t* mbc_decode_from_hex(mbc_token_t key, const char* hex_in, size_t* decoded_size_ptr);

/**
 * Converts raw data into an hexadecimal string.
 * @ret  NUL-terminated hexadecimal string, `NULL` if the string cannot be allocated.
 * @pre  `raw` is an array of bytes; `raw_size > 0`.
 * @post The length of the returned string is double the size of the `raw` array, containing only lower/uppercase (accordingly to `uppercase`) hexadecimal ASCII characters.
 */
char* mbc_raw_to_hex(const uint8_t* raw, size_t raw_size, bool uppercase);

/**
 * Converts hexadecimal string into raw data.
 * @ret  Raw data array, `NULL` if the array cannot be allocated.
 * @pre  `hex` is a non-empty NUL-terminated string containing only lower/uppercase hexadecimal ASCII characters.
 *       If the length of the string is not even the last extra char is discarded.
 * @post `*raw_size_ptr` contains the size of the raw data returned, which is exactly half the length of the `hex` string.
 */
uint8_t* mbc_hex_to_raw(const char* hex, size_t* raw_size_ptr);

#endif
