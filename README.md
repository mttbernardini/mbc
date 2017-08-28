# MBC - Mattyw & MeBeiM symmetric encryption algorithm #

[![license][license-img]][license-link]
[![travis][travis-img]][travis-link]
[![appveyor][appveyor-img]][appveyor-link]
[![codecov][codecov-img]][codecov-link]

This repository provides a C library for the *MBC symmetric encryption algorithm*, plus a parametric CLI program.

This is a remake of the [mbencrypter][1] project, however the new algorithm is not compatible.

## Disclaimer ##

This is a **work in progress**, backwards compatibility is not guaranteed (check `developing` branch).

**The *MBC symmetric encryption algorithm* should not be used in any serious production environment, nor for any kind of cryptographic purposes**, since it is a fast and weak enctyption algorithm, developed just for fun and educational purposes. This software is therefore provided "as is", without warranty of any kind.


## Downloading ##

Clone from this repo:

	~$ git clone https://github.com/mttbernardini/mbc.git
	~$ cd mbc

Or just download one of the available [relaseses][2].


## Building and installing ##

Create a "build" directory and run `cmake` from there:

	~/mbc$ mkdir build
	~/mbc$ cd build
	~/mbc/build$ cmake ..

### Unix systems (Linux, MacOS, Cygwin, etc.) ###

Build & install using `make`:

	~/mbc/build$ sudo make install

### Windows (MinGW) ###

Build & install using `mingw32-make` from an elevated command prompt:

	C:\path\to\mbc\build> mingw32-make install


## Testing ##

All tests can be run using `make` (or `mingw32-make` on Windows):

	~/mbc/build$ make mbc_test

Or selective tests can be run:

	~/mbc/build$ make unit_tests
	~/mbc/build$ ./mbc_unit_tests
	
	~/mbc/build$ make integration_tests
	~/mbc/build$ ./mbc_integration_tests

Random test (only for Unix, requires `bash`):  

	~/mbc/build$ make random_test
	~/mbc/build$ ./mbc_random_test <data-size> <key-size>

*NB: `<data-size>` and `<key-size>` are mandatory and allow suffixes like `k`, `M`, `G` and so on (refer to `dd` documentation).*


## Basic algorithm description ##

 1. Bit-Swap encoding, by MeBeiM:
  - The key is converted into a swap map (see [`/doc/notes.md`][3] for detailed information).
  - The swap map is reduced to generate the `swap_key`, each byte indicating the position of two bits to swap in each byte of data.
  - Each byte of data is processed applying the swaps indicated by the `swap_key`.

 2. Per-Byte XOR encoding:
  - If the key is shorter than data, it will be cycled until the end of data.
  - If data is shorter than the key, it will be cycled until the end of the key.
  - Each byte of data is XORed with each byte of the key.


## Documentation and more ##

All source files provide descriptive comments above functions prototypes and definitions. A complete documentation will soon be available in the `/doc` folder along with additional information.

---
*Copyright © 2017 Matteo Bernardini & Marco Bonelli.*

[1]: https://github.com/mttbernardini/mbencrypter
[2]: https://github.com/mttbernardini/mbc/releases
[3]: https://github.com/mttbernardini/mbc/blob/developing/doc/notes.md#make_oct_key-rationale

[license-img]:   https://img.shields.io/github/license/mttbernardini/mbc.svg
[license-link]:  https://github.com/mttbernardini/mbc/blob/developing/LICENSE
[travis-img]:    https://img.shields.io/travis/mttbernardini/mbc/developing.svg
[travis-link]:   https://travis-ci.org/mttbernardini/mbc
[appveyor-img]:  https://img.shields.io/appveyor/ci/mttbernardini/mbc/developing.svg
[appveyor-link]: https://ci.appveyor.com/project/mttbernardini/mbc/branch/developing
[codecov-img]:   https://img.shields.io/codecov/c/github/mttbernardini/mbc/developing.svg
[codecov-link]:  https://codecov.io/gh/mttbernardini/mbc/branch/developing
