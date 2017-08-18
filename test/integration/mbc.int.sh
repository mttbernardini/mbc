#!/usr/bin/env bash

test_mbc() {
	printf "[0/4] Checking for mbc executable... "

	if [ -x ./mbc ]; then
		printf "OK.\n"
	else
		printf "ERR!\n"
		return 1
	fi

	printf "[1/4] Generating test files ($(echo $1))... "

	# Random file "dummy_raw", raw bytes
	dd ibs=1 count=$1 if=/dev/urandom 2> /dev/null > dummy_raw
	if [ $? -ne 0 ]; then
		printf "ERR!\n"
		return 1
	fi

	# Random file "dummy_hex", hexadecimal ASCII
	dd ibs=1 count=$1 if=/dev/urandom 2> /dev/null | hexdump -ve '1/1 "%02x"' > dummy_hex
	if [ $? -eq 0 ]; then
		printf "OK.\n"
	else
		printf "ERR!\n"
		return 1
	fi

	printf "[2/4] Generating key ($(echo $2))... "

	# Random alphanumeric key including some special characters
	local test_key
	test_key=$( LC_ALL=C tr -dc 'A-Za-z0-9!#$%&)(*+,-./:;<=>?@[\]^_`{|}~' < /dev/urandom | dd ibs=1 count=$2 2> /dev/null )
	if [ $? -eq 0 ]; then
		printf "OK.\n"
	else
		printf "ERR!\n"
		return 1
	fi

	printf "[3/4] Running test... "

	# ENCRYPT/DECRYPT "dummy_raw" into "dummy_raw_out"
	../build/mbc -ek "$test_key" < dummy_raw | ../build/mbc -dk "$test_key" > dummy_raw_out
	if [ $? -ne 0 ]; then
		printf "ERR!\n"
		return 1
	fi

	# DECRYPT/ENCRYPT "dummy_hex" into "dummy_hex_out"
	../build/mbc -dxk "$test_key" < dummy_hex | ../build/mbc -exk "$test_key" > dummy_hex_out
	if [ $? -eq 0 ]; then
		printf "DONE.\n"
	else
		printf "ERR!\n"
		return 1
	fi

	printf "[4/4] Comparing results... "

	# Compare output with original
	cmp -s dummy_raw dummy_raw_out && cmp -s dummy_hex dummy_hex_out
	if [ $? -eq 0 ]; then
		printf "OK.\n"
	else
		printf "ERR!\n"
		return 1
	fi

	return 0
}

test_mbc "$@"
test_result=$?
rm dummy_*

if [ "$test_result" -eq "0" ]; then
	printf "\nTEST PASSED!\n"
	exit 0
else
	printf "\nTEST FAILED!\n"
	exit 1
fi
