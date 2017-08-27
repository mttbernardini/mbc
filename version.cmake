find_package(Git)

if (GIT_FOUND)
	execute_process(
		COMMAND ${GIT_EXECUTABLE} describe --always
		OUTPUT_VARIABLE MBC_VERSION
		OUTPUT_STRIP_TRAILING_WHITESPACE
	)
	STRING(REGEX REPLACE "^v([0-9]+)\\.([0-9]+)-([0-9]+)-[a-z0-9]+$" "\\1.\\2.\\3" MBC_VERSION "${MBC_VERSION}")
	STRING(REGEX REPLACE "^v([0-9]+)\\.([0-9]+)$" "\\1.\\2.0" MBC_VERSION "${MBC_VERSION}")
else()
	set(MBC_VERSION "(unknown version)")
endif()

configure_file(version.c.in version.c ESCAPE_QUOTES @ONLY)
