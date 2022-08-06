project(JustCircleReflectionEnum CXX)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(
		SOURCE_FILES

		enum.cpp
)

add_executable(
		${PROJECT_NAME}
		${SOURCE_FILES}
)

cmake_language(CALL set_compile_options ${PROJECT_NAME})
