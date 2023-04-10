cmake_minimum_required (VERSION 2.8.12)

set (SOURCES lexer.c parser.c)
set (CMAKE_BUILD_TYPE Release)

find_program (BISON bison)
if (NOT BISON)
	message (FATAL_ERROR "Could not find bison")
endif ()

find_program (LEX lex)
if (NOT LEX)
	message (FATAL_ERROR "Could not find lex")
endif ()

add_custom_command (
	OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/lexer.c
	COMMAND ${LEX} -o ${CMAKE_CURRENT_BINARY_DIR}/lexer.c ${CMAKE_CURRENT_SOURCE_DIR}/lexer.l
	DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/lexer.l ${CMAKE_CURRENT_SOURCE_DIR}/parser.y)

add_custom_command (
	OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/parser.c
	COMMAND ${BISON} --defines=${CMAKE_CURRENT_BINARY_DIR}/parser.h -o ${CMAKE_CURRENT_BINARY_DIR}/parser.c ${CMAKE_CURRENT_SOURCE_DIR}/parser.y
	DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/parser.y)

add_executable (upconvert ${SOURCES})
