### Conventions

 - `size_t` is used for sizes and counters.
 - `register` keyword is preferred for counters.
 - `uint8_t*` is used for array of raw bytes (of which the size must be provided/known).
 - `char*` is used for NULL-terminated strings only (instead of `uint8_t*`).
 - `stdint.h` types are preferred.

### Usage

Something like:

    mbc_set_options(...);
    mbc_set_user_key(...);
    data = mbc_raw_to_hex(...); / mbc_hex_to_raw(...);
	mbc_encode(data, ...); / mbc_decode(data, ...);
    mbc_encode(data, ...); / mbc_decode(data, ...);
    ...
    mbc_free();

### `make_oct_key` rationale:

The `swap_mask` starts as `{0,1,2,3,4,5,6,7}`, then for every `i` from `0` to `key_len-1`:

 1. Extract positions of bits to swap:

	    key[i]       : -xxxyyy-
		key[i] >> 4  : 0000-xxx &
		mask (0x07)  : 00000111 =
		l_bit        = 00000xxx

		key[i]       : -xxxyyy-
		key[i] >> 1  : 0-xxxyyy &
		mask (0x07)  : 00000111 =
		r_bit        = 00000yyy

 2. Mirror obtained values if least significant bit of key[i] is 1:

		if ((key[i] & 0x01) && l_bit != (r_bit ^ 0x07)) {
			l_bit ^= 0x07;
			r_bit ^= 0x07;
		}

 3. Swap `swap_mask[l_bit] with `swap_mask[r_bit]`.

 4. At the end of the cycle, `swap_mask` contains the swap mask to be applied to the bits of every byte of data.
 5. The following cycle scans `swap_mask` to extact the shortest array of moves (which will be the real `oct_key`) to perform to obtain the same result.
