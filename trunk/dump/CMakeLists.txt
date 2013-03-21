cmake_minimum_required (VERSION 2.6)
include (CheckCSourceCompiles)

set (SOURCES main.c)

if ("${CMAKE_C_COMPILER_ID}" STREQUAL "Clang")
	set (CMAKE_C_FLAGS "-Wall -g")
elseif ("${CMAKE_C_COMPILER_ID}" STREQUAL "GNU")
	set (CMAKE_C_FLAGS "-Wall -ggdb")
elseif ("${CMAKE_C_COMPILER_ID}" STREQUAL "MSVC")
	set (CMAKE_C_FLAGS "/W3 /D_CRT_SECURE_NO_WARNINGS")
else ()
	message (SEND_ERROR "Unknown compiler ${CMAKE_C_COMPILER_ID}")
endif ()

macro (check_architecture define condition source)
	CHECK_C_SOURCE_COMPILES("
#if ${condition}
int main() {return 0;}
#endif
	" ${define})
	if (${define})
		add_definitions (-D${define})
		list (APPEND SOURCES ${source} ${ARGN})
	endif ()
endmacro ()

check_architecture (UP_I386 "defined(__i386) || defined(_M_IX86)"
	x86cpuid.c x86msr.c)
check_architecture (UP_X86_64 "defined(__x86_64) || defined(_M_AMD64)"
	x86cpuid.c x86msr.c)

set (CMAKE_BUILD_TYPE Release)
add_executable (updump ${SOURCES})
