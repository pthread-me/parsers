cmake_minimum_required(VERSION 4.0)

project(P)

set(CMAKE_EXPORT_COMPILE_COMMANDS on)
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_BUILD_TYPE Debug)

set(SOURCES src/main.cpp src/wave.cpp)

add_executable(main ${SOURCES})
target_include_directories(main PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/include)

target_compile_options(main PRIVATE
  -Wall
  -Werror
  -Wextra
  -Wpedantic
  -O0
)
