test_mbc() {
	printf "[1/4] Generating test files ($(echo $1))... "
	
	# Random file "dummy_raw", raw bytes
	head --bytes=$1 /dev/urandom > dummy_raw

	# Random file "dummy_hex", hexadecimal ASCII
	head --bytes=$1 /dev/urandom | hexdump -ve '1/1 "%02x"' > dummy_hex

	printf "done.\n[2/4] Generating key ($(echo $2))... "

	# Random alphanumeric key including some special characters
	local test_key="$(head /dev/urandom | tr -dc 'A-Za-z0-9!#$%&()*+,-./:;<=>?@[\]^_`{|}~' | head --bytes=$2)"

	printf "done.\n[3/4] Running test... "

	# ENCRYPT/DECRYPT "dummy_raw" into "dummy_raw_out"
	../build/mbc -ek "$test_key" < dummy_raw | ../build/mbc -dk $test_key > dummy_raw_out

	# DECRYPT/ENCRYPT "dummy_hex" into "dummy_hex_out"
	../build/mbc -dxk "$test_key" < dummy_hex | ../build/mbc -exk $test_key > dummy_hex_out

	printf "done.\n[4/4] Comparing results... "

	# Compare output with original
	cmp --quiet dummy_raw dummy_raw_out && cmp --quiet dummy_hex dummy_hex_out
	local result=$?

	printf "done.\n\n"

	if [ "$result" = 0 ]; then
		printf "TEST PASSED!\n"	
	else
		printf "TEST FAILED!\n"
	fi

	rm dummy_*

	return $result
}

test_mbc "$@"
