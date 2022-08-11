cmake_minimum_required(VERSION 3.18)

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
