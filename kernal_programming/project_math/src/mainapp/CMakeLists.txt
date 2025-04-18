add_executable(mainapp main.c)

include_directories(
	${CMAKE_SOURCE_DIR}/inc/libmath
	)
target_link_libraries(mainapp math)
