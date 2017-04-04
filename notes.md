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
    mbc_encode(...); / mbc_decode(...);
    mbc_encode(...); / mbc_decode(...);
    ...
    mbc_free(...);

### `make_oct_key` rationale:

 1. First part:
 	
	    user_key[i]       : -xxxyyy- &
	    bit mask L (0x70) : 01110000 =
	    oct_key[i]        = 0xxx0000
	
 2. Second part:

        user_key[i]       : -xxxyyy- >> 1 =
                            0-xxxyyy &
        bit mask R (0x07) : 00001110 =
        oct_key[i]       += 00000yyy =
        oct_key[i]        : 0xxx0yyy

 3. Mirror values if last bit of user key is set (xxx and yyy become 111-xxx and 111-yyy):

        IF (user_key[i] & 0x01):
        oct_key[i] = ~oct_key[i]
