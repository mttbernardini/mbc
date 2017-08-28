find_package(Git)

set(MBC_VERSION "${HARD_VERSION}")
set(MBC_REVISION "")

if (GIT_FOUND)
	execute_process(
		COMMAND ${GIT_EXECUTABLE} describe --always --long --first-parent --match "v[0-9]*"
		RESULT_VARIABLE GIT_DESCRIBE_EXIT
		OUTPUT_VARIABLE GIT_DESCRIBE_OUT
		OUTPUT_STRIP_TRAILING_WHITESPACE
	)
	if (GIT_DESCRIBE_EXIT EQUAL 0)
		# Version format conventions
		if (GIT_DESCRIBE_OUT MATCHES "^[a-f0-9]+$") # fallback on only hash
			set(MBC_VERSION "#dev-${GIT_DESCRIBE_OUT}")
		else()
			string(REGEX REPLACE "^v([0-9]+\\.[0-9]+(\\.[0-9]+)?).*" # standard version
				"\\1" MBC_VERSION "${GIT_DESCRIBE_OUT}")
			string(REGEX REPLACE "^v.*-([0-9]+)-g[a-f0-9]+$" # extract revision
				"\\1" MBC_REVISION "${GIT_DESCRIBE_OUT}")
		endif()
		if (NOT HARD_VERSION STREQUAL "" AND NOT MBC_VERSION STREQUAL HARD_VERSION)
			message(AUTHOR_WARNING
				"Hardcoded version in CMakeLists.txt and `git describe` computed version do not match! Using computed version.\n"
				"Hardcoded: ${HARD_VERSION} - Computed: ${MBC_VERSION}\n"
			)
		endif()
	endif()
endif()

if(NOT (MBC_REVISION STREQUAL "0" OR MBC_REVISION STREQUAL ""))
	set(MBC_VERSION "${MBC_VERSION}_r${MBC_REVISION}")
endif()

if(MBC_VERSION STREQUAL "")
	set(MBC_VERSION "(unknown version)")
endif()

message("-- MBC version: ${MBC_VERSION}")

if(NOT EXISTS version.c)
	file(WRITE version.c "const char* MBC_VERSION = \"${MBC_VERSION}\";\n")
else()
	file(STRINGS version.c SOURCE_LINE LIMIT_COUNT 1)
	if(NOT SOURCE_LINE MATCHES "const char\\* MBC_VERSION = \"${MBC_VERSION}\"")
		file(WRITE version.c "const char* MBC_VERSION = \"${MBC_VERSION}\";\n")
	endif()
endif()
