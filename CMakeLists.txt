cmake_minimum_required(VERSION 3.23)
project(Pong)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_OBJC_COMPILE_OBJECT False)
add_subdirectory(vendor/sdl)
add_subdirectory(vendor/sdl_image)

include_directories(vendor/sdl/include)
include_directories(vendor/sdl_image/include)

add_executable(Pong src/main.cpp
src/App.cpp
src/Ball.cpp
src/Pad.cpp
src/MiddleBar.cpp
src/Score.cpp
)

target_link_libraries(Pong SDL3::SDL3 SDL3_image::SDL3_image)

set_target_properties(
		Pong
		PROPERTIES
		LIBRARY_OUTPUT_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/Application/lib"
		RUNTIME_OUTPUT_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/Application/bin"
		ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/Application/arch"
)
