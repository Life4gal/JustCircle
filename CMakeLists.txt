cmake_minimum_required(VERSION 3.16)

find_program(COMPILER_CIRCLE circle)
if (${CMAKE_HOST_UNIX})
	if (NOT EXISTS "${COMPILER_CIRCLE}")
		list(APPEND CMAKE_PROGRAM_PATH "/usr/bin")
		find_program(COMPILER_CIRCLE circle)
	endif (NOT EXISTS "${COMPILER_CIRCLE}")
else()
	message(FATAL_ERROR "circle compiler is not support at your computer.")
endif (${CMAKE_HOST_UNIX})

if (EXISTS "${COMPILER_CIRCLE}")
	set(CMAKE_CXX_COMPILER "${COMPILER_CIRCLE}")
else ()
	message(FATAL_ERROR "circle compiler is not found at your computer.")
endif ()

project(JustCircle CXX)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
#set(CMAKE_CXX_STANDARD 20)

macro(set_compile_options target_name)
	message("Setting compile parameters for project ${target_name}...")
	target_compile_options(
		${target_name}
		PUBLIC

		-Wall -Wextra --pedantic -Werror -std=c++20
	)
endmacro()

add_subdirectory(src/reflection)
