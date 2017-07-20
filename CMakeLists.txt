cmake_minimum_required (VERSION 2.8.8) # NB: 3.4 is needed for WINDOWS_EXPORT_ALL_SYMBOLS to work
project("MBC" LANGUAGES C)

# In-source build protection
if (${CMAKE_SOURCE_DIR} STREQUAL ${CMAKE_BINARY_DIR})
    message(FATAL_ERROR "In-source builds not allowed. Please make a new directory (called a build directory) and run CMake from there. You may need to remove CMakeCache.txt.")
endif()

# Version number (not needed for now)
#set(MBC_VERSION_MAJOR 0)
#set(MBC_VERSION_MINOR 1)

# Compiler settings
set_property(GLOBAL PROPERTY C_STANDARD 99)
set(CMAKE_C_FLAGS "-Wall -Wextra")

# Add the library
add_library(libmbc SHARED "src/libmbc.c")
target_include_directories(libmbc PUBLIC "src/")
set_target_properties(libmbc PROPERTIES PREFIX "") # avoids "liblibmbc.so", but maybe this is not the best way to do it.
set_target_properties(libmbc PROPERTIES WINDOWS_EXPORT_ALL_SYMBOLS TRUE) # for MSVC

# Add sources
add_executable(mbc "src/mbc.c")
add_executable(mbc_test "src/test.c")
add_executable(mbc_test_file "src/test_file.c")

# Link em
target_link_libraries(mbc libmbc)
target_link_libraries(mbc_test libmbc)
target_link_libraries(mbc_test_file libmbc)

# Install
install(TARGETS libmbc
	RUNTIME DESTINATION bin
	LIBRARY DESTINATION lib
	ARCHIVE DESTINATION lib
)
install(TARGETS mbc DESTINATION bin)
install(FILES "src/libmbc.h" DESTINATION include)
