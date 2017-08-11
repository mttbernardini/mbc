enable_testing()

# Group all the unit tests in a single variable
file(GLOB unit_tests_src "test/unit/*.unit.c")

# Add main executable for all unit tests
add_executable(unit_tests EXCLUDE_FROM_ALL "test/unit/main.c" ${unit_tests_src})
target_include_directories(unit_tests PUBLIC "src" "test")

# Workaround for building the test before running it with CMake's CTest... horrible ikr
# add_test(unit_tests_build "${CMAKE_COMMAND}" --build ${CMAKE_BINARY_DIR} --target unit_tests)
# add_test(unit_tests_run unit_tests)
# set_tests_properties(unit_tests_run PROPERTIES DEPENDS unit_tests_build)

ctest_build(TARGET unit_tests)
#set(CTEST_BUILD_TARGET unit_tests)
add_test(unit_tests unit_tests)
