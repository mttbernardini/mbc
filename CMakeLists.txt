cmake_minimum_required (VERSION 2.8)
project("mbc")

# In-source build protection
if ( ${CMAKE_SOURCE_DIR} STREQUAL ${CMAKE_BINARY_DIR} )
    message( FATAL_ERROR "In-source builds not allowed. Please make a new directory (called a build directory) and run CMake from there. You may need to remove CMakeCache.txt." )
endif()

# Version number (not needed for now)
#set(MBC_VERSION_MAJOR 0)
#set(MBC_VERSION_MINOR 1)

# Compile in C99 with all warnings
add_definitions(-std=c99)
add_definitions(-Wall)

# Add library
add_library(libmbc SHARED "src/libmbc.c")
set_target_properties(libmbc PROPERTIES PREFIX "")

# Add sources
add_executable(mbc "src/mbc.c")
add_executable(mbc_test "src/test.c")
add_executable(mbc_test_file "src/test_file.c")

# Link em
target_link_libraries(mbc libmbc)
target_link_libraries(mbc_test libmbc)
target_link_libraries(mbc_test_file libmbc)

# Install
install(TARGETS libmbc DESTINATION lib)
install(TARGETS mbc DESTINATION bin)
install(FILES "src/libmbc.h" DESTINATION include)
