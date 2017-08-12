### Conventions

 - `size_t` is used for sizes and counters.
 - `register` keyword is preferred for counters.
 - `uint8_t*` is used for array of raw bytes (of which the size must be provided/known).
 - `char*` is used for NUL-terminated strings only (instead of `uint8_t*`).
 - `stdint.h` types are preferred.

### `make_oct_key` rationale:

The `dummy_byte` starts as `{0,1,2,3,4,5,6,7}`, then for every `i` from `0` to `key_len-1`:

 1. Extract positions of bits to swap:

	    key[i]       : -xxxyyy-
		key[i] >> 4  : 0000-xxx &
		mask (0x07)  : 00000111 =
		l_bit        = 00000xxx

		key[i]       : -xxxyyy-
		key[i] >> 1  : 0-xxxyyy &
		mask (0x07)  : 00000111 =
		r_bit        = 00000yyy

 2. Mirror obtained values if least significant bit of `key[i]` is `1`:

		if ((key[i] & 0x01) && l_bit != (r_bit ^ 0x07)) {
			l_bit ^= 0x07;
			r_bit ^= 0x07;
		}

 3. Swap `dummy_byte[l_bit]` with `dummy_byte[r_bit]`.

 4. At the end of the cycle, `dummy_byte` contains the shuffled bits, and each of them is identified by its value.
 5. The following cycle builds up a swap map from the `dummy_byte`: the bit in position `i` will be in position `swap_map[i]` after the swap part of the algorithm is executed.
 6. The final `for` cycle scans `swap_map` to extract the shortest array of moves (which will be the real `oct_key`) to perform to obtain the same result.
