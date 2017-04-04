#ifndef _MBC_H_
#define _MBC_H_

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>

enum mbc_options;

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

#endif
