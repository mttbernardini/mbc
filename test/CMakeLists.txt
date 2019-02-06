# Group all test file names in apropriate variables
file(GLOB unit_tests_src "test/unit/*.unit.c")
file(GLOB integration_tests_src "test/integration/*.int.c")
file(GLOB memory_tests_src "test/memory/*.mem.c")

# Add main executables for tests
add_executable(unit_tests EXCLUDE_FROM_ALL "test/unit/main.c" ${unit_tests_src})
add_executable(integration_tests EXCLUDE_FROM_ALL "test/integration/main.c" ${integration_tests_src})
add_executable(memory_tests EXCLUDE_FROM_ALL "test/memory/main.c" ${memory_tests_src})
target_include_directories(unit_tests PUBLIC "src" "test")
target_include_directories(integration_tests PUBLIC "src" "test")
target_include_directories(memory_tests PUBLIC "src" "test")

target_link_libraries(integration_tests libmbc)

# Add bash script
add_custom_command(
	OUTPUT mbc_random_test
	DEPENDS test/integration/mbc.int.sh
	COMMAND ${CMAKE_COMMAND} -E copy
		$(CMAKE_SOURCE_DIR)/test/integration/mbc.int.sh
		mbc_random_test
)
add_custom_target(random_test DEPENDS mbc mbc_random_test)

# Add mbc_ prefix to executables
set_target_properties(unit_tests PROPERTIES OUTPUT_NAME mbc_unit_tests)
set_target_properties(integration_tests PROPERTIES OUTPUT_NAME mbc_integration_tests)
set_target_properties(memory_tests PROPERTIES OUTPUT_NAME mbc_memory_tests)


if (UNIX)
	add_custom_target(mbc_test
		COMMAND ./mbc_unit_tests
		COMMAND ./mbc_memory_tests
		COMMAND ./mbc_integration_tests
		COMMAND ./mbc_random_test 512k 32k
		DEPENDS unit_tests integration_tests memory_tests random_test
		COMMENT "Running test..."
	)
else()
	add_custom_target(mbc_test
		COMMAND ./mbc_unit_tests
		COMMAND ./mbc_memory_tests
		COMMAND ./mbc_integration_tests
		DEPENDS unit_tests integration_tests memory_tests
		COMMENT "Running test..."
	)
endif()
