find_package(Git)

set(GIT_DESCRIBE_EXIT 1)
set(MBC_VERSION "")

if (GIT_FOUND)
	execute_process(
		COMMAND ${GIT_EXECUTABLE} describe --always --long --first-parent
		RESULT_VARIABLE GIT_DESCRIBE_EXIT
		OUTPUT_VARIABLE MBC_VERSION
		OUTPUT_STRIP_TRAILING_WHITESPACE
	)
endif()

if((NOT GIT_DESCRIBE_EXIT EQUAL 0) AND EXISTS ${ROOT}/VERSION)
	file(STRINGS ${ROOT}/VERSION MBC_VERSION LIMIT_COUNT 1)
endif()

if(MBC_VERSION STREQUAL "")
	set(MBC_VERSION "(unknown version)")
endif()


# Version format conventions
STRING(REGEX REPLACE  # master standard release
	"^v([0-9]+)\\.([0-9]+)-([0-9]+)-g[a-f0-9]+$"
	"\\1.\\2.\\3"
	MBC_VERSION "${MBC_VERSION}"
)
STRING(REGEX REPLACE  # master variant release
	"^v([0-9]+)\\.([0-9]+)-([a-zA-Z0-9]+)-([0-9]+)-g[a-f0-9]+$"
	"\\1.\\2.\\4-\\3"
	MBC_VERSION "${MBC_VERSION}"
)
STRING(REGEX REPLACE  # master release fallback (no rev. number)
	"^v([0-9]+)\\.([0-9]+)$"
	"\\1.\\2"
	MBC_VERSION "${MBC_VERSION}"
)
STRING(REGEX REPLACE  # master variant release fallback (no rev. number)
	"^v([0-9]+)\\.([0-9]+)-([a-zA-Z0-9]+)$"
	"\\1.\\2-\\3"
	MBC_VERSION "${MBC_VERSION}"
)
STRING(REGEX REPLACE  # development release (commit hash based)
	"^([a-f0-9]+)$"
	"#git-\\1"
	MBC_VERSION "${MBC_VERSION}"
)

configure_file(version.c.in version.c ESCAPE_QUOTES @ONLY)
