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
