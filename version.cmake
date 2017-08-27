find_package(Git)

set(GIT_DESCRIBE_EXIT 1)

if (GIT_FOUND)
	execute_process(
		COMMAND ${GIT_EXECUTABLE} describe --always
		RESULT_VARIABLE GIT_DESCRIBE_EXIT
		OUTPUT_VARIABLE MBC_VERSION
		OUTPUT_STRIP_TRAILING_WHITESPACE
	)
endif()

if(NOT GIT_DESCRIBE_EXIT EQUAL 0)
	if (EXISTS ${ROOT}/VERSION)
		file(STRINGS ${ROOT}/VERSION MBC_VERSION LIMIT_COUNT 1)
	else()
		set(MBC_VERSION "(unknown version)")
	endif()
endif()

STRING(REGEX REPLACE "^v([0-9]+)\\.([0-9]+)-([0-9]+)-[a-f0-9]+$" "\\1.\\2.\\3" MBC_VERSION "${MBC_VERSION}")
STRING(REGEX REPLACE "^v([0-9]+)\\.([0-9]+)$" "\\1.\\2.0" MBC_VERSION "${MBC_VERSION}")
STRING(REGEX REPLACE "^([a-f0-9]+)$" "#\\1" MBC_VERSION "${MBC_VERSION}")

configure_file(version.c.in version.c ESCAPE_QUOTES @ONLY)
