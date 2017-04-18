# MBC - Symmetric Encryption Algorithm by Mattyw&MeBeiM #

[![Build Status](https://travis-ci.org/mttbernardini/mbc.svg?branch=developing)](https://travis-ci.org/mttbernardini/mbc)

**THIS IS A WORK IN PROGRESS** *(check `developing` branch)*.

This repository will provide a C library for the *mbc symmetric encryption algorithm*, plus a parametric CLI program for `stdio` usage.

NB: This is a remake of the [mbencrypter][1] project, however the new alghorithm is not compatible.

## Installation ##

### Unix systems (Linux, MacOS, etc) ###

Clone the repository and enter the base directory:

	$ git clone https://github.com/mttbernardini/mbc.git
	$ cd mbc

Create a "build" directory and run `cmake` from there:

	$ mkdir build
	$ cd build
	$ cmake ..

Build & install using `make`:

	$ sudo make install

## Algorithm Description ##

### 1. Bit-Swap encoding, by MeBeiM ###
- The key is converted into a swap mask (see [notes.md][2] for the detailed mechanism).
- The swap mask is reduced to generate a swap key, each byte indicating the position of two bits to swap in each byte of data.
- Each byte of data is processed applying the swaps indicated by the swap key.

### 2. Per-Byte XOR encoding ###
- If the key is shorter than data, it will be cycled until the end of data.
- If data is shorter than the key, it will be cycled until the end of the key.
- Each byte of data is XORed with each byte of the key.

---
© Matteo Bernardini & Marco Bonelli, 2017


[1]: https://github.com/mttbernardini/mbencrypter
[2]: https://github.com/mttbernardini/mbc/blob/developing/notes.md#make_oct_key-rationale
