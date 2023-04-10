cmake_minimum_required (VERSION 2.8.12)
include (CheckCSourceCompiles)

set (SOURCES main.c platform.c)
set (CMAKE_BUILD_TYPE Release)

if ("${CMAKE_SYSTEM_NAME}" STREQUAL "Darwin")
	list (APPEND SOURCES osx.c)
elseif ("${CMAKE_SYSTEM_NAME}" STREQUAL "Linux")
	list (APPEND SOURCES linux.c)
elseif ("${CMAKE_SYSTEM_NAME}" STREQUAL "SunOS")
	list (APPEND SOURCES solaris.c)
elseif ("${CMAKE_SYSTEM_NAME}" STREQUAL "Windows")
	list (APPEND SOURCES windows.c)
else ()
	message (SEND_ERROR "Unknown platform ${CMAKE_SYSTEM_NAME}")
endif ()

if ("${CMAKE_C_COMPILER_ID}" STREQUAL "Clang")
	set (CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall")
elseif ("${CMAKE_C_COMPILER_ID}" STREQUAL "GNU")
	set (CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall")
elseif ("${CMAKE_C_COMPILER_ID}" STREQUAL "MSVC")
	set (CMAKE_C_FLAGS "${CMAKE_C_FLAGS} /W3 /D_CRT_SECURE_NO_WARNINGS")
elseif ("${CMAKE_C_COMPILER_ID}" STREQUAL "SunPro")
	set (CMAKE_C_FLAGS "${CMAKE_C_FLAGS}")
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

add_executable (updump ${SOURCES})
