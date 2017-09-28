FIND_PROGRAM(GZIP gzip)

if (NOT GZIP)
	message(WARNING "gzip executable not found. man pages will be installed uncompressed.")
	install(FILES man/mbc.1 DESTINATION share/man/man1)
	install(FILES man/libmbc.3 DESTINATION share/man/man3)
else()

	add_custom_command(
		OUTPUT mbc.1.gz
		DEPENDS man/mbc.1
		COMMAND ${GZIP} -c -9 ${CMAKE_SOURCE_DIR}/man/mbc.1 > mbc.1.gz
		COMMENT "Generating mbc.1.gz"
	)
	add_custom_command(
		OUTPUT libmbc.3.gz
		DEPENDS man/libmbc.3
		COMMAND ${GZIP} -c -9 ${CMAKE_SOURCE_DIR}/man/libmbc.3 > libmbc.3.gz
		COMMENT "Generating libmbc.3.gz"
	)

	add_custom_target(man ALL DEPENDS mbc.1.gz libmbc.3.gz)
	install(FILES ${CMAKE_BINARY_DIR}/mbc.1.gz DESTINATION share/man/man1)
	install(FILES ${CMAKE_BINARY_DIR}/libmbc.3.gz DESTINATION share/man/man3)

endif()
