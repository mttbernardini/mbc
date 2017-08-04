#ifndef LIBMBC_H_INCLUDED
#define LIBMBC_H_INCLUDED

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>

/**
 * Sets the value of global `user_key` and calculates global `oct_key`.
 * @ret `true` on success, `false` otherwise.
 * @pre `key` is an array of bytes; `key_size` > 0
 */
bool mbc_set_user_key(const uint8_t* key, size_t key_size);

/**
 * Frees any dynamically allocated variables (basically the global keys).
 * @post `user_key` and `oct_key` are NULL pointers.
 */
void mbc_free(void);

/**
 * Encodes data in place.
 * @pre  A key has been correctly set; `data` is a dynamically allocated array of bytes; `data_size > 0`.
 * @post `data` contains encoded bytes.
 */
void mbc_encode_inplace(uint8_t* data, size_t data_size);

/**
 * Decodes data in place.
 * @pre  A key has been correctly set; `data` is a dynamically allocated array of bytes; `data_size > 0`.
 * @post `data` contains decoded bytes.
 */
void mbc_decode_inplace(uint8_t* data, size_t data_size);

/**
 * Encodes data in a new array.
 * @ret  Encoded data as array, `NULL` if the array cannot be allocated.
 * @pre  A key has been corectly set; `data` is an array of bytes; `data_size > 0`.
 */
uint8_t* mbc_encode(const uint8_t* data, size_t data_size);

/**
 * Decodes data in a new array.
 * @ret  Decoded data as array, `NULL` if the array cannot be allocated.
 * @pre  A key has been corectly set; `data` is an array of bytes; `data_size > 0`.
 */
uint8_t* mbc_decode(const uint8_t* data, size_t data_size);

/**
 * Encodes data to hexadecimal output.
 * @ret  NUL-terminated hexadecimal string, `NULL` if the string cannot be allocated.
 * @pre  A key has been corectly set; `raw_in` is an array of bytes; `raw_size > 0`.
 * @post The length of the returned string is even, containing only lower/uppercase (accordingly to `uppercase`) hexadecimal ASCII characters.
 */
char* mbc_encode_to_hex(const uint8_t* raw_in, size_t raw_size, bool uppercase);

/**
 * Decodes data from hexadecimal input.
 * @ret  Raw decoded data array, `NULL` if the array cannot be allocated.
 * @pre  A key has been corectly set;
 *       `hex_in` is a non-empty NUL-terminated string containing only lower/uppercase hexadecimal ASCII characters.
 *       If the length of the string is not even the last extra char is discarded.
 * @post `*decoded_size_ptr` contains the size of decoded data returned.
 */
uint8_t* mbc_decode_from_hex(const char* hex_in, size_t* decoded_size_ptr);

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
