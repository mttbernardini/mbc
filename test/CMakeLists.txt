# Group all test file names in apropriate variables
file(GLOB unit_tests_src "test/unit/*.unit.c")
file(GLOB integration_tests_src "test/integration/*.int.c")

# Add main executables for tests
add_executable(unit_tests EXCLUDE_FROM_ALL "test/unit/main.c" ${unit_tests_src})
add_executable(integration_tests EXCLUDE_FROM_ALL "test/integration/main.c" ${integration_tests_src})
target_include_directories(unit_tests PUBLIC "src" "test")
target_include_directories(integration_tests PUBLIC "src" "test")

target_link_libraries(integration_tests libmbc)

configure_file("test/integration/mbc.int.sh" "mbc_random_test" COPYONLY)

# Add mbc_ prefix to executables
set_target_properties(unit_tests PROPERTIES OUTPUT_NAME mbc_unit_tests)
set_target_properties(integration_tests PROPERTIES OUTPUT_NAME mbc_integration_tests)

add_custom_target(
	mbc_test
	COMMAND ./mbc_unit_tests
	COMMAND ./mbc_integration_tests
	DEPENDS mbc unit_tests integration_tests
	COMMENT "Running test..."
)
