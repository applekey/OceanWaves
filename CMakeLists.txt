# CMake entry point
cmake_minimum_required (VERSION 2.8)
project (MyScad)

find_package(OpenGL REQUIRED)


include_directories(
	${OPENGL_INCLUDE_DIRS}
	${CMAKE_CURRENT_SOURCE_DIR}/glfw-3.0.4.bin.WIN32/include/GLFW
	${CMAKE_CURRENT_SOURCE_DIR}/glew-1.10.0/include
	${CMAKE_CURRENT_SOURCE_DIR}/glm-0.9.4.0/
	)

set(ALL_LIBS
	${CMAKE_CURRENT_SOURCE_DIR}/glfw-3.0.4.bin.WIN32/lib-msvc110/glfw3.lib
	${CMAKE_CURRENT_SOURCE_DIR}/glew-1.10.0/lib/Release/Win32/glew32.lib
	${OPENGL_LIBRARY}
	)

add_definitions(-std=c99)


set(PROJECT_HEADERS
	helperClass.h
	)

set(PROJECT_FILES
	main.cpp
	display.frag
	display.vert
	shapes.mscad
	helperClass.cpp
	controls.h
	common.h
	compute.vert
	wave.h
	fft.h
	fft.vert
	)

add_executable(MyScad ${PROJECT_FILES} ${PROJECT_HEADERS})
target_link_libraries(	MyScad ${ALL_LIBS})