cmake_minimum_required(VERSION 3.23)
project(Pong)

set(CMAKE_CXX_STANDARD 20)

add_subdirectory(vendor/sdl)
target_compile_options(SDL2 PUBLIC "-w")

include_directories(vendor/sdl/include)

file(GLOB PONG_CPP
		"${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp"
		"${CMAKE_CURRENT_SOURCE_DIR}/src/*/*.cpp"
		)

add_executable(Pong ${PONG_CPP} src/Ball.cpp src/Ball.h src/Pad.cpp src/Pad.h)
target_link_libraries(Pong SDL2::SDL2 SDL2main)

set_target_properties(
		Pong SDL2
		PROPERTIES
		LIBRARY_OUTPUT_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/Application/lib"
		RUNTIME_OUTPUT_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/Application/bin"
		ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/Application/arch"
)