cmake_minimum_required (VERSION 2.8.8) # NB: 3.4 is needed for WINDOWS_EXPORT_ALL_SYMBOLS to work
project("MBC" LANGUAGES C)

# Prevent in-source build
if (${CMAKE_SOURCE_DIR} STREQUAL ${CMAKE_BINARY_DIR})
    message(FATAL_ERROR "In-source builds not allowed. Please make a new directory (called a build directory) and run CMake from there. You may need to remove CMakeCache.txt.")
endif()

cmake_policy(SET CMP0042 OLD) # we don't care about this policy. Set to suppress warning on MacOS

# Version number (not needed for now)
#set(MBC_VERSION_MAJOR 0)
#set(MBC_VERSION_MINOR 1)

# Compiler settings
set_property(GLOBAL PROPERTY C_STANDARD 99)
set(CMAKE_C_FLAGS "-Wall -Wextra")

# Add library
add_library(libmbc SHARED "src/libmbc.c")
target_include_directories(libmbc PUBLIC "src/")
set_target_properties(libmbc PROPERTIES
	OUTPUT_NAME "mbc" # avoids "liblibmbc" name
	WINDOWS_EXPORT_ALL_SYMBOLS TRUE # for MSVC
)
if (CMAKE_BUILD_TYPE STREQUAL "Coverage")
	set_target_properties(libmbc PROPERTIES
		COMPILE_FLAGS "--coverage"
		LINK_FLAGS "--coverage"
	)
endif()

# Add executable
add_executable(mbc "src/mbc.c")

# Linking
target_link_libraries(mbc libmbc)

# Install
install(TARGETS libmbc
	RUNTIME DESTINATION bin
	LIBRARY DESTINATION lib
	ARCHIVE DESTINATION lib
)
install(TARGETS mbc DESTINATION bin)
install(FILES "src/libmbc.h" DESTINATION include)

# Include cmake file for tests
include("test/CMakeLists.txt")
