cmake_minimum_required (VERSION 2.6)

set (SOURCES parse.c)
set (CMAKE_BUILD_TYPE Release)

find_program (CHICKEN chicken)
if (NOT CHICKEN)
	message (FATAL_ERROR "Could not find CHICKEN Scheme compiler (www.call-cc.org/)")
endif ()

find_path (CHICKEN_HEADERS chicken/chicken.h)
if (CHICKEN_HEADERS)
	include_directories (${CHICKEN_HEADERS}/chicken)
else ()
	message (FATAL_ERROR "Could not find chicken.h header")
endif ()

find_library (CHICKEN_LIBRARY chicken)
if (NOT CHICKEN_LIBRARY)
	message (FATAL_ERROR "Could not find chicken library")
endif ()

foreach (CSOURCE ${SOURCES})
	string (REPLACE ".c" ".scm" SCMSOURCE ${CSOURCE})
	add_custom_command (
		OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/${CSOURCE}
		COMMAND ${CHICKEN} ${CMAKE_CURRENT_SOURCE_DIR}/${SCMSOURCE} -output-file ${CMAKE_CURRENT_BINARY_DIR}/${CSOURCE}
		DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/${SCMSOURCE})
endforeach (CSOURCE ${SOURCES})

add_executable (upparse ${SOURCES})
target_link_libraries (upparse chicken)
