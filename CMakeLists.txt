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

# Add sources
add_executable(mbc_test "src/test.c")
add_library(mbc SHARED "src/libmbc.c")

# Link em
target_link_libraries(mbc_test mbc)

# Install
install(TARGETS mbc DESTINATION lib)
install(TARGETS mbc_test DESTINATION bin)
install(FILES "src/libmbc.h" DESTINATION include)
