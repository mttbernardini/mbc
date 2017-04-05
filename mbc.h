#ifndef MBC_H_INCLUDED
#define MBC_H_INCLUDED

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>

/**
 * Sets the value of global `user_key` and calculates global `oct_key`.
 * @ret `true` if success, `false` otherwise.
 */
bool mbc_set_user_key(const uint8_t* key, size_t key_size);

/**
 * Frees any dynamically allocated variables (basically the global keys).
 */
void mbc_free();

/**
 * Encodes data.
 * @ret  Encoded data as array, `NULL` if the array cannot be `malloc`ated.
 * @pre  A key has been corectly set; `data` is an array of bytes; `data_size > 0`.
 */
uint8_t* mbc_encode(const uint8_t* data, size_t data_size);

/**
 * Decodes data.
 * @ret  Decoded data as array, `NULL` if the array cannot be `malloc`ated.
 * @pre  A key has been corectly set; `data` is an array of bytes; `data_size > 0`.
 */
uint8_t* mbc_decode(const uint8_t* data, size_t data_size);

/**
 * Converts raw data into an hexadecimal string.
 * @ret  NULL-terminated hexadecimal string.
 * @pre  `raw` is an array of bytes; `raw_size > 0`.
 * @post The length of the returned string is even, containing only lower/uppercase (accordingly to `uppercase`) hexadecimal ASCII characters.
 */
char* mbc_raw_to_hex(const uint8_t* raw, size_t raw_size, bool uppercase);

/**
 * Converts hexadecimal string into raw data.
 * @ret  Raw data array.
 * @pre  `hex` is a NULL-terminated string containing only lower/uppercase hexadecimal ASCII characters, and its length should be > 0.
 * @post `*raw_size_ptr` contains the size of the raw data returned.
 */
uint8_t* mbc_hex_to_raw(const char* hex, size_t* raw_size_ptr);

#endif
